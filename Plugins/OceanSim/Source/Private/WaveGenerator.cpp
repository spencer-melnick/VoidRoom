#include "WaveGenerator.h"

#include "Containers/DynamicRHIResourceArray.h"
#include "RenderingThread.h"
#include "RenderGraphBuilder.h"
#include "PixelShaderUtils.h"

#include "BoxMullerShader.h"
#include "InitialComponentsShader.h"

void FWaveGenerator::Initialize(FIntPoint Dimensions)
{
	BufferSize = Dimensions;
	unsigned int NumElements = BufferSize.X * BufferSize.Y;

	// Allocate a large array for noise data
	TArray<FVector4> NoiseData;
	NoiseData.Init(FVector4(), NumElements);

	// Generate some random uniform noise
	for (unsigned int i = 0; i < NumElements; i++)
	{
		NoiseData[i] = FVector4(FMath::FRandRange(0, 1), FMath::FRandRange(0, 1), FMath::FRandRange(0, 1), FMath::FRandRange(0, 1));
	}

	FIntVector BoxMullerGroupCount = FComputeShaderUtils::GetGroupCount(BufferSize, FBoxMullerShader::ThreadsPerGroupDimension);
	FIntVector InitialComponentsGroupCount = FComputeShaderUtils::GetGroupCount(BufferSize, FInitialComponentsShader::ThreadsPerGroupDimension);
	
	ENQUEUE_RENDER_COMMAND(SceneDrawCompletion)
	([this, NumElements, BoxMullerGroupCount, InitialComponentsGroupCount, NoiseData]
	(FRHICommandListImmediate& CommandListImmediate)
	{
		FRDGBuilder GraphBuilder(CommandListImmediate);

		FRDGBufferDesc NoiseBufferDesc = FRDGBufferDesc::CreateStructuredDesc(sizeof(FVector4), NumElements);
		FRDGBufferRef NoiseBuffer = GraphBuilder.CreateBuffer(NoiseBufferDesc, TEXT("NoiseBuffer"));
		FRDGBufferUAVRef NoiseBufferUAV = GraphBuilder.CreateUAV(NoiseBuffer);
		// FRDGBufferSRVRef NoiseBufferSRV = GraphBuilder.CreateSRV(NoiseBuffer);

		FRHIResourceCreateInfo CreateInfo;
		InitialComponentsBuffer = RHICreateStructuredBuffer(sizeof(FVector4), NumElements, BUF_UnorderedAccess | BUF_ShaderResource, CreateInfo);
		InitialComponentsBufferUAV = RHICreateUnorderedAccessView(InitialComponentsBuffer, false, false);

		FBoxMullerShader::FParameters* BoxMullerParameters = GraphBuilder.AllocParameters<FBoxMullerShader::FParameters>();
		BoxMullerParameters->NoiseBuffer = NoiseBufferUAV;
		BoxMullerParameters->NoiseBufferSize = BufferSize;

		/*FInitialComponentsShader::FParameters* InitialComponentsParameters = GraphBuilder.AllocParameters<FInitialComponentsShader::FParameters>();
		InitialComponentsParameters->NoiseBuffer = NoiseBufferSRV;
		InitialComponentsParameters->OutputBuffer = InitialComponentsBufferUAV;
		InitialComponentsParameters->CommonParameters.BufferSize = BufferSize;
		InitialComponentsParameters->CommonParameters.Amplitude = 1;
		InitialComponentsParameters->CommonParameters.PatchLength = 64;
		InitialComponentsParameters->CommonParameters.Gravity = 9.81;
		InitialComponentsParameters->CommonParameters.WindSpeed = 40;
		InitialComponentsParameters->CommonParameters.WindDirection = FVector2D(1, 1).GetSafeNormal(0.001);*/

		TShaderMapRef<FBoxMullerShader> BoxMullerShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
		// TShaderMapRef<FInitialComponentsShader> InitialComponentsShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

		GraphBuilder.AddPass(RDG_EVENT_NAME("Gaussian Noise Generation"), BoxMullerParameters, ERDGPassFlags::Compute,
		[&](FRHICommandList& CommandList)
		{
			/*FRDGBufferRef Temp = BoxMullerParameters->NoiseBuffer->GetParent();
			FRHIResource* RHITemp = Temp->GetRHI();
			
			FStructuredBufferRHIRef NoiseBufferRHI = static_cast<FRHIStructuredBuffer*>(BoxMullerParameters->NoiseBuffer->GetParent()->GetRHI());
			
			FVector4* NoiseBufferData = static_cast<FVector4*>(RHILockStructuredBuffer(NoiseBufferRHI, 0, sizeof(FVector4), EResourceLockMode::RLM_WriteOnly));
			FMemory::Memcpy(NoiseBufferData, NoiseData.GetData(), NumElements);
			RHIUnlockStructuredBuffer(NoiseBufferRHI);*/

			// FComputeShaderUtils::Dispatch(CommandList, *BoxMullerShader, *BoxMullerParameters, BoxMullerGroupCount);
		});
		
		// FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("Initial Height Componenets Generation"), *BoxMullerShader,
			// BoxMullerParameters, BoxMullerGroupCount);

		GraphBuilder.Execute();
	});
}

