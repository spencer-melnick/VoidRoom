#include "WaveGenerator.h"

#include "Modules/ModuleManager.h"
#include "RenderingThread.h"
#include "RenderGraphBuilder.h"
#include "PixelShaderUtils.h"
#include "Containers/DynamicRHIResourceArray.h"
#include "RenderTargetPool.h"

#include "BoxMullerShader.h"
#include "InitialComponentsShader.h"
#include "RealtimeComponentsShader.h"
#include "ButterflyShader.h"
#include "CopyShader.h"
#include "BitReverseCopyShader.h"
#include "ScaleInvertShader.h"


DECLARE_GPU_STAT(WaveComponentGeneration)
DECLARE_GPU_STAT(WaveFFTs)
DECLARE_GPU_STAT(WaveChannelCombination)


FWaveGenerator::~FWaveGenerator()
{
	// Just in case the object is somehow still hooked to the render thread
	StopRendering();
}


void FWaveGenerator::Initialize(uint32 LengthInPoints, UTextureRenderTarget2D* DisplacementTarget, UTextureRenderTarget2D* SlopeTarget)
{
	Length = LengthInPoints;
	NumSteps = static_cast<uint32>(FMath::RoundToInt(FMath::Log2(Length)));
	BufferSize = FIntPoint(Length, Length);
	StartTime = GRenderingRealtimeClock.GetCurrentTime();

	GenerateGaussianNoise();
	GenerateBitReversal();
	CalculateButterflyOperations();

	bAreParametersUpToDate = false;

	DisplacementRenderTarget = DisplacementTarget;
	SlopeRenderTarget = SlopeTarget;
}

void FWaveGenerator::BeginRendering()
{
	if (ResolvedSceneColorHandle.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot attach Wave Generator to rendering thread - already attached"));
		return;
	}

	const FName RendererModuleName("Renderer");
	IRendererModule* RendererModule = FModuleManager::GetModulePtr<IRendererModule>(RendererModuleName);
	
	if (RendererModule != nullptr)
	{
		ResolvedSceneColorHandle = RendererModule->GetResolvedSceneColorCallbacks().AddRaw(this, &FWaveGenerator::OnRender);
	}
}

void FWaveGenerator::StopRendering()
{
	if (!ResolvedSceneColorHandle.IsValid())
	{
		return;
	}

	const FName RendererModuleName("Renderer");
	IRendererModule* RendererModule = FModuleManager::GetModulePtr<IRendererModule>(RendererModuleName);
	if (RendererModule)
	{
		RendererModule->GetResolvedSceneColorCallbacks().Remove(ResolvedSceneColorHandle);
	}

	ResolvedSceneColorHandle.Reset();
}

void FWaveGenerator::SetParameters(FGenerationParameters NewParameters)
{
	ParameterLock.Lock();
	GenerationParameters = NewParameters;
	bAreParametersUpToDate = false;
	ParameterLock.Unlock();
}


void FWaveGenerator::OnRender(FRHICommandListImmediate& RHICmdList, FSceneRenderTargets& SceneContext)
{	
	if (!InitialComponentsTexture.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Wave Generator is missing initial height components texture - make sure to call Initialize() first"));
		return;
	}

	FRDGBuilder GraphBuilder(RHICmdList);

	// Lock the generation parameters and copy to the shader parameters
	ParameterLock.Lock();
	bool bCopyAreParametersUpToDate = bAreParametersUpToDate;
	
	FOceanShaderCommonParameters CommonParameters;
	CommonParameters.BufferSize = BufferSize;
	CommonParameters.Amplitude = GenerationParameters.Amplitude;
	CommonParameters.PatchLength = GenerationParameters.PatchLength;
	CommonParameters.Gravity = GenerationParameters.Gravity;
	CommonParameters.WindSpeed = GenerationParameters.WindSpeed;
	CommonParameters.WindDirection = GenerationParameters.WindDirection.GetSafeNormal(0.001);
	float FoamLambda = GenerationParameters.FoamLambda;

	// No matter what happens, at the end of all the compute passes, the parameters will be up to date
	bAreParametersUpToDate = true;
	ParameterLock.Unlock();

	// Do initial component generation if any parameters have changed
	if (!bCopyAreParametersUpToDate)
	{
		FInitialComponentsShader::FParameters* InitialPassParameters = GraphBuilder.AllocParameters<FInitialComponentsShader::FParameters>();
		InitialPassParameters->NoiseTexture = GaussianNoiseTextureSRV;
		InitialPassParameters->ComponentTexture = InitialComponentsTextureUAV;
		InitialPassParameters->CommonParameters = CommonParameters;

		TShaderMapRef<FInitialComponentsShader> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
		FIntVector GroupCount = FComputeShaderUtils::GetGroupCount(BufferSize, FInitialComponentsShader::ThreadsPerGroupDimension);

		FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("Initial Wave Height Component Generation"),
			*ComputeShader, InitialPassParameters, GroupCount);
	}

	// Generate realtime components
	FIntVector GroupCount = FComputeShaderUtils::GetGroupCount(BufferSize, FRealtimeComponentsShader::ThreadsPerGroupDimension);

	// Allocate a ton of buffers for all the component variants
	FRDGBufferDesc ComponentsBufferDesc = FRDGBufferDesc::CreateStructuredDesc(sizeof(float) * 2, Length * Length);
	FRDGBufferRef ComponentsBuffer = GraphBuilder.CreateBuffer(ComponentsBufferDesc, TEXT("WaveHeightComponentsBuffer"));
	FRDGBufferUAVRef ComponentsBufferUAV = GraphBuilder.CreateUAV(ComponentsBuffer);

	FRDGBufferRef DisplacementXBuffer = GraphBuilder.CreateBuffer(ComponentsBufferDesc, TEXT("WaveDisplacementXBuffer"));
	FRDGBufferUAVRef DisplacementXBufferUAV = GraphBuilder.CreateUAV(DisplacementXBuffer);

	FRDGBufferRef DisplacementYBuffer = GraphBuilder.CreateBuffer(ComponentsBufferDesc, TEXT("WaveDisplacementYBuffer"));
	FRDGBufferUAVRef DisplacementYBufferUAV = GraphBuilder.CreateUAV(DisplacementYBuffer);

	FRDGBufferRef SlopeXBuffer = GraphBuilder.CreateBuffer(ComponentsBufferDesc, TEXT("WaveSlopeXBuffer"));
	FRDGBufferUAVRef SlopeXBufferUAV = GraphBuilder.CreateUAV(SlopeXBuffer);

	FRDGBufferRef SlopeYBuffer = GraphBuilder.CreateBuffer(ComponentsBufferDesc, TEXT("WaveSlopeYBuffer"));
	FRDGBufferUAVRef SlopeYBufferUAV = GraphBuilder.CreateUAV(SlopeYBuffer);

	{
		RDG_GPU_STAT_SCOPE(GraphBuilder, WaveComponentGeneration)
		
		FRealtimeComponentsShader::FParameters* PassParameters = GraphBuilder.AllocParameters<FRealtimeComponentsShader::FParameters>();
		PassParameters->InitialComponents = InitialComponentsTextureSRV;
		PassParameters->HeightComponentsBuffer = ComponentsBufferUAV;
		PassParameters->DisplacementXBuffer = DisplacementXBufferUAV;
		PassParameters->DisplacementYBuffer = DisplacementYBufferUAV;
		PassParameters->SlopeXBuffer = SlopeXBufferUAV;
		PassParameters->SlopeYBuffer = SlopeYBufferUAV;
		PassParameters->CommonParameters = CommonParameters;
		PassParameters->Time = GRenderingRealtimeClock.GetCurrentTime() - StartTime;

		TShaderMapRef<FRealtimeComponentsShader> ComponentsShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

		FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("Wave Height Component Generation"), *ComponentsShader, PassParameters, GroupCount);
	}

	// FFT passes
	// TODO: Optimize FFTs using different LODs for displacement and normals
	// TODO: Possibly combine X and Y gradient FFTs

	{
		RDG_GPU_STAT_SCOPE(GraphBuilder, WaveFFTs)
		
		DoFFT2(GraphBuilder, ComponentsBufferUAV);
		DoFFT2(GraphBuilder, DisplacementXBufferUAV);
		DoFFT2(GraphBuilder, DisplacementYBufferUAV);
		DoFFT2(GraphBuilder, SlopeXBufferUAV);
		DoFFT2(GraphBuilder, SlopeYBufferUAV);
	}

	{
		RDG_GPU_STAT_SCOPE(GraphBuilder, WaveChannelCombination)
		
		// Scale and invert results as appropriate to get the height texture

		GroupCount = FIntVector(Length / 8, Length / 8, 1);

		FRDGTextureDesc HeightTextureDesc = FRDGTextureDesc::Create2DDesc(BufferSize, EPixelFormat::PF_FloatRGBA, FClearValueBinding::Black,
			TexCreate_None, TexCreate_ShaderResource | TexCreate_UAV, false);
		FRDGTextureRef HeightTexture = GraphBuilder.CreateTexture(HeightTextureDesc, TEXT("WaveDisplacementTexture"));

		FRDGTextureDesc SlopeTextureDesc = FRDGTextureDesc::Create2DDesc(BufferSize, EPixelFormat::PF_G16R16F, FClearValueBinding::Black,
			TexCreate_None, TexCreate_ShaderResource | TexCreate_UAV, false);
		FRDGTextureRef SlopeTexture = GraphBuilder.CreateTexture(SlopeTextureDesc, TEXT("WaveSlopeTexture"));

		FRDGBufferSRVDesc ComponentsBufferSRVDesc(ComponentsBuffer);
		FRDGBufferSRVRef ComponentsBufferSRV = GraphBuilder.CreateSRV(ComponentsBufferSRVDesc);

		FRDGBufferSRVDesc DisplacementXBufferSRVDesc(DisplacementXBuffer);
		FRDGBufferSRVRef DisplacementXBufferSRV = GraphBuilder.CreateSRV(DisplacementXBufferSRVDesc);

		FRDGBufferSRVDesc DisplacementYBufferSRVDesc(DisplacementYBuffer);
		FRDGBufferSRVRef DisplacementYBufferSRV = GraphBuilder.CreateSRV(DisplacementYBufferSRVDesc);

		FRDGBufferSRVDesc SlopeXBufferSRVDesc(SlopeXBuffer);
		FRDGBufferSRVRef SlopeXBufferSRV = GraphBuilder.CreateSRV(SlopeXBufferSRVDesc);

		FRDGBufferSRVDesc SlopeYBufferSRVDesc(SlopeYBuffer);
		FRDGBufferSRVRef SlopeYBufferSRV = GraphBuilder.CreateSRV(SlopeYBufferSRVDesc);

		auto* ScaleInvertParameters = GraphBuilder.AllocParameters<FScaleInvertShader::FParameters>();
		ScaleInvertParameters->HeightBuffer = ComponentsBufferSRV;
		ScaleInvertParameters->DisplacementXBuffer = DisplacementXBufferSRV;
		ScaleInvertParameters->DisplacementYBuffer = DisplacementYBufferSRV;
		ScaleInvertParameters->SlopeXBuffer = SlopeXBufferSRV;
		ScaleInvertParameters->SlopeYBuffer = SlopeYBufferSRV;
		ScaleInvertParameters->DisplacementTexture = GraphBuilder.CreateUAV(HeightTexture);
		ScaleInvertParameters->SlopeTexture = GraphBuilder.CreateUAV(SlopeTexture);
		ScaleInvertParameters->BufferSize = BufferSize;
		ScaleInvertParameters->FoamLambda = FoamLambda;

		TShaderMapRef<FScaleInvertShader> ScaleInvertShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

		FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("Wave Scale Invert IFFT Pass"), *ScaleInvertShader, ScaleInvertParameters, GroupCount);


		// Extract the texture from the graph
		TRefCountPtr<IPooledRenderTarget> PooledDisplacementTarget = GetPooledRenderTarget(DisplacementRenderTarget);
		TRefCountPtr<IPooledRenderTarget> PooledSlopeTarget = GetPooledRenderTarget(SlopeRenderTarget);

		FRDGTextureRef DisplacementRenderTargetRDG = GraphBuilder.RegisterExternalTexture(PooledDisplacementTarget, TEXT("WaveDisplacementRenderTarget"));
		FRDGTextureRef SlopeRenderTargetRDG = GraphBuilder.RegisterExternalTexture(PooledSlopeTarget, TEXT("WaveSlopeRenderTarget"));

		AddCopyTexturePass(GraphBuilder, HeightTexture, DisplacementRenderTargetRDG);
		AddCopyTexturePass(GraphBuilder, SlopeTexture, SlopeRenderTargetRDG);
	}

	GraphBuilder.Execute();
}


void FWaveGenerator::GenerateGaussianNoise()
{
	uint32 NumElements = BufferSize.X * BufferSize.Y * 4;
	FIntVector GroupCount = FComputeShaderUtils::GetGroupCount(BufferSize, FCopyShader::ThreadsPerGroupDimension);

	TArray<FFloat16> InputData;
	InputData.Init(FFloat16(), sizeof(FFloat16) * 4 * NumElements);

	for (uint32 i = 0; i < NumElements; i++)
	{
		InputData[i] = FMath::FRandRange(0, 1);
	}

	ENQUEUE_RENDER_COMMAND(SceneDrawCompletion)
	([this, &InputData, GroupCount, NumElements]
	(FRHICommandListImmediate& CommandListImmediate)
	{
		FRDGBuilder GraphBuilder(CommandListImmediate);

		// Create uniform noise texture
		FRHIResourceCreateInfo CreateInfo;
		FTexture2DRHIRef UniformNoiseTexture = RHICreateTexture2D(BufferSize.X, BufferSize.Y, PF_FloatRGBA, 1, 1,
			TexCreate_ShaderResource, CreateInfo);
		FShaderResourceViewRHIRef UniformNoiseTextureSRV = RHICreateShaderResourceView(UniformNoiseTexture, 0);

		// Allocate Gaussian noise texture (this is kept in the object for future usage)
		GaussianNoiseTexture = RHICreateTexture2D(BufferSize.X, BufferSize.Y, PF_FloatRGBA, 1, 1,
			TexCreate_ShaderResource | TexCreate_UAV, CreateInfo);
		GaussianNoiseTextureSRV = RHICreateShaderResourceView(GaussianNoiseTexture, 0);
		FUnorderedAccessViewRHIRef GaussianNoiseTextureUAV = RHICreateUnorderedAccessView(GaussianNoiseTexture);

		// Allocate initial components texture
		InitialComponentsTexture = RHICreateTexture2D(BufferSize.X, BufferSize.Y, PF_A32B32G32R32F, 1, 1,
			TexCreate_ShaderResource | TexCreate_UAV, CreateInfo);
		InitialComponentsTextureSRV = RHICreateShaderResourceView(InitialComponentsTexture, 0);
		InitialComponentsTextureUAV = RHICreateUnorderedAccessView(InitialComponentsTexture);
		

		// Copy uniform noise data to texture
		uint32 Stride;
		FFloat16* TextureData = static_cast<FFloat16*>(RHILockTexture2D(UniformNoiseTexture, 0, EResourceLockMode::RLM_WriteOnly, Stride, false));
		FMemory::Memcpy(TextureData, InputData.GetData(), NumElements * sizeof(FFloat16));
		RHIUnlockTexture2D(UniformNoiseTexture, 0, false);

		FBoxMullerShader::FParameters* BoxMullerPassParameters = GraphBuilder.AllocParameters<FBoxMullerShader::FParameters>();
		BoxMullerPassParameters->InputTexture = UniformNoiseTextureSRV;
		BoxMullerPassParameters->OutputTexture = GaussianNoiseTextureUAV;

		TShaderMapRef<FBoxMullerShader> BoxMullerShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

		FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("Box Muller Pass"), *BoxMullerShader, BoxMullerPassParameters, GroupCount);

		GraphBuilder.Execute();
	});

	// Since we generated the input data on the main thread, we need to wait for the rendering thread to be done before we continue
	// otherwise the data will be unallocated before it can be copied to the texture
	FlushRenderingCommands();
	bHasGaussianNoise = true;
}

void FWaveGenerator::GenerateBitReversal()
{
	TResourceArray<FSwapIndex> LookupData;
	LookupData.Init(FSwapIndex(), Length / 2);

	uint32 Index = 0;

	for (uint32 i = 0; i < Length; i++)
	{
		FSwapIndex Swap;
		Swap.IndexA = i;
		Swap.IndexB = BitReverse(i, NumSteps);

		if (Swap.IndexA < Swap.IndexB)
		{
			LookupData[Index++] = Swap;
		}
	}

	UE_LOG(LogTemp, Display, TEXT("Generated %i unique bit-reversed swaps"), Index);

	FRHIResourceCreateInfo CreateInfo;
	CreateInfo.ResourceArray = &LookupData;
	BitReverseBuffer = RHICreateStructuredBuffer(sizeof(FSwapIndex), sizeof(FSwapIndex) * Length / 2, BUF_ShaderResource, CreateInfo);
	BitReverseBufferSRV = RHICreateShaderResourceView(BitReverseBuffer);
}


void FWaveGenerator::CalculateButterflyOperations()
{
	TResourceArray<FButterflyOperation> Operations;
	Operations.Init(FButterflyOperation(0, 0, FVector2D(1, 0)), NumSteps * Length / 2);

	uint32 i = 0;
	
	for (uint32 s = 0; s < NumSteps; s++)
	{
		uint32 m = static_cast<uint32>(FMath::Pow(2, s + 1));
		FVector2D wm = ImaginaryExponent(2 * PI / m);
		for (uint32 k = 0; k < Length; k += m)
		{
			FVector2D w = FVector2D(1, 0);
			for (uint32 j = 0; j < m / 2; j ++)
			{
				Operations[i++] = FButterflyOperation(k + j, k + j + m / 2, w);
				w = ComplexMultiply(w, wm);
			}
		}
	}

	ENQUEUE_RENDER_COMMAND(SceneDrawCompletion)
	([this, &Operations]
	(FRHICommandListImmediate& RHICmdList)
	{
		FRHIResourceCreateInfo CreateInfo;
		CreateInfo.ResourceArray = &Operations;
		ButterflyBuffer = RHICreateStructuredBuffer(sizeof(FButterflyOperation), sizeof(FButterflyOperation) * Operations.Num(),
			BUF_ShaderResource, CreateInfo);
		ButterflyBufferSRV = RHICreateShaderResourceView(ButterflyBuffer);
	});

	FlushRenderingCommands();

	UE_LOG(LogTemp, Display, TEXT("Precomputed %i butterfly operations"), i);
}

FVector2D FWaveGenerator::ImaginaryExponent(float Theta)
{
	return FVector2D(FMath::Cos(Theta), FMath::Sin(Theta));
}

FVector2D FWaveGenerator::ComplexMultiply(FVector2D A, FVector2D B)
{
	return FVector2D(A.X * B.X - A.Y * B.Y, A.X * B.Y + A.Y * B.X);
}

uint32 FWaveGenerator::BitReverse(uint32 Value, uint32 NumBits)
{
	uint32 NewValue = 0;
	
	for (uint32 i = 0; i < NumBits; i++)
	{
		uint32 j = NumBits - (i + 1);
		bool BitValue = (Value & (1 << i)) >> i;
		NewValue |= BitValue << j;
	}

	return NewValue;
}

TRefCountPtr<IPooledRenderTarget> FWaveGenerator::GetPooledRenderTarget(UTextureRenderTarget2D* RenderTarget)
{
	FRenderTarget* RenderTargetResource = RenderTarget->GetRenderTargetResource();
	FTexture2DRHIRef RenderTargetRHI = RenderTargetResource->GetRenderTargetTexture();

	FSceneRenderTargetItem RenderTargetItem;
	RenderTargetItem.TargetableTexture = RenderTargetRHI;
	RenderTargetItem.ShaderResourceTexture = RenderTargetRHI;

	FPooledRenderTargetDesc RenderTargetDesc = FPooledRenderTargetDesc::Create2DDesc(RenderTargetResource->GetSizeXY(),
		RenderTargetRHI->GetFormat(), FClearValueBinding::Black, TexCreate_None, TexCreate_RenderTargetable |
		TexCreate_ShaderResource | TexCreate_UAV, false);
	TRefCountPtr<IPooledRenderTarget> PooledRenderTarget;
	GRenderTargetPool.CreateUntrackedElement(RenderTargetDesc, PooledRenderTarget, RenderTargetItem);

	return PooledRenderTarget;
}


template <FWaveGenerator::EFFTDirection Direction>
void FWaveGenerator::DoFFT(FRDGBuilder& GraphBuilder, FRDGBufferUAVRef DataSet)
{
	FString DirectionString;

	if (Direction == EFFTDirection::FFT_Vertical)
	{
		DirectionString = "Vertical";
	}
	else
	{
		DirectionString = "Horizontal";
	}
	
	// Do a bit reverse copy for the (I)FFT
	FIntVector GroupCount = FComputeShaderUtils::GetGroupCount(FIntPoint(Length, Length / 2), FBitReverseCopyShader<Direction>::ThreadsPerGroupDimension);

	auto CopyPassParameters = GraphBuilder.AllocParameters<typename FBitReverseCopyShader<Direction>::FParameters>();
	CopyPassParameters->DataBuffer = DataSet;
	CopyPassParameters->BufferSize = BufferSize;
	CopyPassParameters->BitReversalLookup = BitReverseBufferSRV;

	TShaderMapRef<FBitReverseCopyShader<Direction>> CopyShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

	FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("Wave Bit Reverse Copy %s", *DirectionString), *CopyShader, CopyPassParameters, GroupCount);


	// Get shader and group count
	TShaderMapRef<FButterflyShader<Direction>> ButterflyShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	GroupCount = FIntVector(Length / 8, Length / (8 * 2), 1);

	// Do a pass for each step of the (I)FFT
	for (uint32 i = 0; i < NumSteps; i++)
	{
		auto ButterflyPassParameters = GraphBuilder.AllocParameters<typename FButterflyShader<Direction>::FParameters>();
		ButterflyPassParameters->DataBuffer = DataSet;
		ButterflyPassParameters->Operations = ButterflyBufferSRV;
		ButterflyPassParameters->BufferSize = BufferSize;
		ButterflyPassParameters->OperationStartIndex = i * Length / 2;

		FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("Wave %s IFFT Iteration %i", *DirectionString, i), *ButterflyShader, ButterflyPassParameters,
			GroupCount);
	}
}

void FWaveGenerator::DoFFT2(FRDGBuilder& GraphBuilder, FRDGBufferUAVRef DataSet)
{
	DoFFT<EFFTDirection::FFT_Horizontal>(GraphBuilder, DataSet);
	DoFFT<EFFTDirection::FFT_Vertical>(GraphBuilder, DataSet);
}



