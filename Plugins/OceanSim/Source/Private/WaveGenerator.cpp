#include "WaveGenerator.h"

#include "Modules/ModuleManager.h"
#include "RenderingThread.h"
#include "RenderGraphBuilder.h"
#include "PixelShaderUtils.h"
#include "Containers/DynamicRHIResourceArray.h"

#include "BoxMullerShader.h"
#include "InitialComponentsShader.h"
#include "RealtimeComponentsShader.h"
#include "ButterflyShader.h"
#include "CopyShader.h"
#include "BitReverseCopyShader.h"
#include "ScaleInvertShader.h"

FWaveGenerator::~FWaveGenerator()
{
	// Just in case the object is somehow still hooked to the render thread
	StopRendering();
}


void FWaveGenerator::Initialize(uint32 LengthInPoints)
{
	Length = LengthInPoints;
	NumSteps = static_cast<uint32>(FMath::RoundToInt(FMath::Log2(Length)));
	BufferSize = FIntPoint(Length, Length);
	StartTime = GRenderingRealtimeClock.GetCurrentTime();

	GenerateGaussianNoise();
	GenerateBitReversal();
	CalculateButterflyOperations();
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

void FWaveGenerator::OnRender(FRHICommandListImmediate& RHICmdList, FSceneRenderTargets& SceneContext)
{	
	if (!InitialComponentsTexture.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Wave Generator is missing initial height components texture - make sure to call Initialize() first"));
		return;
	}

	FRDGBuilder GraphBuilder(RHICmdList);

	FOceanShaderCommonParameters CommonParameters;
	CommonParameters.BufferSize = BufferSize;
	CommonParameters.Amplitude = 4;
	CommonParameters.PatchLength = 1000;
	CommonParameters.Gravity = 9.81;
	CommonParameters.WindSpeed = 40;
	CommonParameters.WindDirection = FVector2D(1, 1).GetSafeNormal(0.001);

	// Do initial component generation if any parameters have changed
	if (!bAreParametersUpToDate)
	{
		FInitialComponentsShader::FParameters* InitialPassParameters = GraphBuilder.AllocParameters<FInitialComponentsShader::FParameters>();
		InitialPassParameters->NoiseTexture = GaussianNoiseTextureSRV;
		InitialPassParameters->ComponentTexture = InitialComponentsTextureUAV;
		InitialPassParameters->CommonParameters = CommonParameters;

		TShaderMapRef<FInitialComponentsShader> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
		FIntVector GroupCount = FComputeShaderUtils::GetGroupCount(BufferSize, FInitialComponentsShader::ThreadsPerGroupDimension);

		FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("Initial Wave Height Component Generation"),
			*ComputeShader, InitialPassParameters, GroupCount);

		FRDGTextureDesc OutputTextureDesc = FRDGTextureDesc::Create2DDesc(BufferSize, EPixelFormat::PF_FloatRGBA, FClearValueBinding::BlackMaxAlpha,
			TexCreate_None, TexCreate_ShaderResource | TexCreate_UAV | TexCreate_RenderTargetable, false);
		FRDGTextureRef OutputTexture = GraphBuilder.CreateTexture(OutputTextureDesc, TEXT("ComponentDebugTexture"));

		FCopyShader::FParameters* CopyPassParameters = GraphBuilder.AllocParameters<FCopyShader::FParameters>();
		CopyPassParameters->InputTexture = InitialComponentsTextureSRV;
		CopyPassParameters->OutputTexture = GraphBuilder.CreateUAV(OutputTexture);
		CopyPassParameters->Bias = 0;
		CopyPassParameters->Scale = 1;

		TShaderMapRef<FCopyShader> CopyShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

		FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("Copy Component Texture Pass"), *CopyShader, CopyPassParameters, GroupCount);
	}

	// Generate realtime components
	FIntVector GroupCount = FComputeShaderUtils::GetGroupCount(BufferSize, FRealtimeComponentsShader::ThreadsPerGroupDimension);

	FRDGBufferDesc ComponentsBufferDesc = FRDGBufferDesc::CreateStructuredDesc(sizeof(float) * 2, Length * Length);
	FRDGBufferRef ComponentsBuffer = GraphBuilder.CreateBuffer(ComponentsBufferDesc, TEXT("WaveHeightComponentsBuffer"));
	FRDGBufferUAVRef ComponentsBufferUAV = GraphBuilder.CreateUAV(ComponentsBuffer);

	FRealtimeComponentsShader::FParameters* PassParameters = GraphBuilder.AllocParameters<FRealtimeComponentsShader::FParameters>();
	PassParameters->InitialComponents = InitialComponentsTextureSRV;
	PassParameters->OutputBuffer = ComponentsBufferUAV;
	PassParameters->CommonParameters = CommonParameters;
	PassParameters->Time = GRenderingRealtimeClock.GetCurrentTime() - StartTime;

	TShaderMapRef<FRealtimeComponentsShader> ComponentsShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

	FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("Realtime Wave Height Component Generation"), *ComponentsShader, PassParameters, GroupCount);

	// Vertical FFT
	{
		// Do a bit reverse copy for the (I)FFT
		GroupCount = FComputeShaderUtils::GetGroupCount(FIntPoint(Length, Length / 2), FBitReverseCopyShaderVertical::ThreadsPerGroupDimension);

		auto CopyPassParameters = GraphBuilder.AllocParameters<FBitReverseCopyShaderVertical::FParameters>();
		CopyPassParameters->DataBuffer = ComponentsBufferUAV;
		CopyPassParameters->BufferSize = BufferSize;
		CopyPassParameters->BitReversalLookup = BitReverseBufferSRV;

		TShaderMapRef<FBitReverseCopyShaderVertical> CopyShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

		FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("Wave Bit Reverse Copy Vertical"), *CopyShader, CopyPassParameters, GroupCount);


		// Get vertical shader and group count
		TShaderMapRef<FButterflyShaderVertical> ButterflyShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
		GroupCount = FComputeShaderUtils::GetGroupCount(FIntPoint(Length, Length / 2), FButterflyShaderVertical::ThreadsPerGroupDimension);

		// Do a pass for each step of the (I)FFT
		for (uint32 i = 0; i < NumSteps; i++)
		{
			auto ButterflyPassParameters = GraphBuilder.AllocParameters<FButterflyShaderVertical::FParameters>();
			ButterflyPassParameters->DataBuffer = ComponentsBufferUAV;
			ButterflyPassParameters->Operations = ButterflyBufferSRV;
			ButterflyPassParameters->BufferSize = BufferSize;
			ButterflyPassParameters->OperationStartIndex = i * Length / 2;

			FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("Wave Vertical IFFT Iteration %i", i), *ButterflyShader, ButterflyPassParameters,
				GroupCount);
		}
	}

	// Horizontal FFT
	{
		// Do a bit reverse copy for the (I)FFT
		GroupCount = FComputeShaderUtils::GetGroupCount(FIntPoint(Length, Length / 2), FBitReverseCopyShaderHorizontal::ThreadsPerGroupDimension);

		auto CopyPassParameters = GraphBuilder.AllocParameters<FBitReverseCopyShaderHorizontal::FParameters>();
		CopyPassParameters->DataBuffer = ComponentsBufferUAV;
		CopyPassParameters->BufferSize = BufferSize;
		CopyPassParameters->BitReversalLookup = BitReverseBufferSRV;

		TShaderMapRef<FBitReverseCopyShaderHorizontal> CopyShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

		FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("Wave Bit Reverse Copy Vertical"), *CopyShader, CopyPassParameters, GroupCount);


		// Get horizontal shader and group count
		TShaderMapRef<FButterflyShaderHorizontal> ButterflyShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
		GroupCount = FComputeShaderUtils::GetGroupCount(FIntPoint(Length, Length / 2), FButterflyShaderHorizontal::ThreadsPerGroupDimension);

		// Do a pass for each step of the (I)FFT
		for (uint32 i = 0; i < NumSteps; i++)
		{
			auto ButterflyPassParameters = GraphBuilder.AllocParameters<FButterflyShaderHorizontal::FParameters>();
			ButterflyPassParameters->DataBuffer = ComponentsBufferUAV;
			ButterflyPassParameters->Operations = ButterflyBufferSRV;
			ButterflyPassParameters->BufferSize = BufferSize;
			ButterflyPassParameters->OperationStartIndex = i * Length / 2;

			FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("Wave Vertical IFFT Iteration %i", i), *ButterflyShader, ButterflyPassParameters,
				GroupCount);
		}
	}
	

	{	
		// Scale and invert results as appropriate to get the height texture

		GroupCount = FComputeShaderUtils::GetGroupCount(FIntPoint(Length, Length), FScaleInvertShader::ThreadsPerGroupDimension);

		FRDGTextureDesc HeightTextureDesc = FRDGTextureDesc::Create2DDesc(BufferSize, EPixelFormat::PF_FloatRGBA, FClearValueBinding::Black, TexCreate_None,
			TexCreate_ShaderResource | TexCreate_UAV, false);
		FRDGTextureRef HeightTexture = GraphBuilder.CreateTexture(HeightTextureDesc, TEXT("WaveHeightTexture"));
		FRDGTextureUAVRef HeightTextureUAV = GraphBuilder.CreateUAV(HeightTexture);

		FRDGBufferSRVDesc ComponentsBufferSRVDesc(ComponentsBuffer);
		FRDGBufferSRVRef ComponentsBufferSRV = GraphBuilder.CreateSRV(ComponentsBufferSRVDesc);

		auto* ScaleInvertParameters = GraphBuilder.AllocParameters<FScaleInvertShader::FParameters>();
		ScaleInvertParameters->DataBuffer = ComponentsBufferSRV;
		ScaleInvertParameters->OutputTexture = HeightTextureUAV;
		ScaleInvertParameters->BufferSize = BufferSize;

		TShaderMapRef<FScaleInvertShader> ScaleInvertShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

		GraphBuilder.AddPass(RDG_EVENT_NAME("Debug view results"), ScaleInvertParameters, ERDGPassFlags::Compute,
		[&ScaleInvertShader, &ScaleInvertParameters, GroupCount, &ComponentsBuffer, this](FRHICommandList& RHICmdList)
		{
			FComputeShaderUtils::Dispatch(RHICmdList, *ScaleInvertShader, *ScaleInvertParameters, GroupCount);

			if (!bAreParametersUpToDate)
			{
				FTexture2DRHIRef OutputTextureRHI = static_cast<FRHITexture2D*>(ScaleInvertParameters->OutputTexture->GetParent()->GetRHI());
				uint32 Stride;
				FFloat16 * ReadData = static_cast<FFloat16*>(RHILockTexture2D(OutputTextureRHI, 0, EResourceLockMode::RLM_ReadOnly, Stride, false));
				for (uint32 i = 0; i < 4 * Length * Length; i++)
				{
					UE_LOG(LogTemp, Display, TEXT("%f"), static_cast<float>(ReadData[i]));
				}
				RHIUnlockTexture2D(OutputTextureRHI, 0, false);
			}
		});
	}

	bAreParametersUpToDate = true;

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
		InitialComponentsTexture = RHICreateTexture2D(BufferSize.X, BufferSize.Y, PF_FloatRGBA, 1, 1,
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

		
		// Create a debug output texture
		FRDGTextureDesc OutputTextureDesc = FRDGTextureDesc::Create2DDesc(BufferSize, EPixelFormat::PF_FloatRGBA, FClearValueBinding::BlackMaxAlpha,
			TexCreate_None, TexCreate_ShaderResource | TexCreate_UAV | TexCreate_RenderTargetable, false);
		FRDGTextureRef OutputTexture = GraphBuilder.CreateTexture(OutputTextureDesc, TEXT("GaussianNoiseDebugTexture"));

		FCopyShader::FParameters* CopyPassParameters = GraphBuilder.AllocParameters<FCopyShader::FParameters>();
		CopyPassParameters->InputTexture = GaussianNoiseTextureSRV;
		CopyPassParameters->OutputTexture = GraphBuilder.CreateUAV(OutputTexture);
		CopyPassParameters->Bias = 0.5;
		CopyPassParameters->Scale = 1 / 2.5;

		TShaderMapRef<FCopyShader> CopyShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

		FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("Copy Gaussian Texture Pass"), *CopyShader, CopyPassParameters, GroupCount);

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


