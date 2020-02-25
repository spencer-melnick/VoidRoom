#include "NoiseGenerator.h"

#include "ConvertShader.h"
#include "BoxMullerShader.h"

#include "Containers/DynamicRHIResourceArray.h"
#include "PixelShaderUtils.h"

void FNoiseGenerator::AllocateResources(FIntPoint BufferSize)
{
	NoiseBufferSize = BufferSize;
	unsigned int NumElements = NoiseBufferSize.X * NoiseBufferSize.Y;

	// Allocate a large array for noise data
	TResourceArray<FVector4> NoiseData;
	NoiseData.Init(FVector4(), NumElements);

	// Generate some random uniform noise
	for (unsigned int i = 0; i < NumElements; i++)
	{
		NoiseData[i] = FVector4(FMath::FRandRange(0, 1), FMath::FRandRange(0, 1), FMath::FRandRange(0, 1), FMath::FRandRange(0, 1));
	}

	// Assign data to creation info
	FRHIResourceCreateInfo CreateInfo;
	CreateInfo.ResourceArray = &NoiseData;

	// Create buffer
	NoiseBuffer = RHICreateStructuredBuffer(sizeof(FVector4), sizeof(FVector4) * NumElements, BUF_ShaderResource | BUF_UnorderedAccess, CreateInfo);
	NoiseBufferUAV = RHICreateUnorderedAccessView(NoiseBuffer, false, false);
	NoiseBufferSRV = RHICreateShaderResourceView(NoiseBuffer);

	OutputTexture = RHICreateTexture2D(NoiseBufferSize.X, NoiseBufferSize.Y, PF_FloatRGBA, 1, 1,
		TexCreate_ShaderResource | TexCreate_UAV, CreateInfo);
	OutputTextureUAV = RHICreateUnorderedAccessView(OutputTexture);
}


void FNoiseGenerator::Execute()
{	
	// Set up noise compute shader parameters
	FBoxMullerShader::FParameters NoisePassParameters;
	NoisePassParameters.NoiseBuffer = NoiseBufferUAV;
	NoisePassParameters.NoiseBufferSize = NoiseBufferSize;

	// Set up convert compute shader parameters
	FConvertShader::FParameters ConvertPassParameters;
	ConvertPassParameters.OutputTexture = OutputTextureUAV;
	ConvertPassParameters.InputBuffer = NoiseBufferSRV;
	ConvertPassParameters.InputBufferSize = NoiseBufferSize;


	// Retrieve shader from global shader map
	TShaderMapRef<FBoxMullerShader> NoiseComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	TShaderMapRef<FConvertShader> ConvertComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	

	// Calculate group count
	FIntVector GroupCount(FMath::DivideAndRoundUp(NoiseBufferSize.X, FBoxMullerShader::ThreadsPerGroupDimension),
		FMath::DivideAndRoundUp(NoiseBufferSize.Y, FBoxMullerShader::ThreadsPerGroupDimension),
		1);

	uint32 NumElements = NoiseBufferSize.X * NoiseBufferSize.Y;

	UE_LOG(LogTemp, Display, TEXT("Dispatching Gaussian noise compute request"))

	// Execute deferred in the rendering thread
	ENQUEUE_RENDER_COMMAND(SceneDrawCompletion)
	([NoiseComputeShader, NoisePassParameters, ConvertComputeShader, ConvertPassParameters, GroupCount, NumElements, this]
	(FRHICommandListImmediate& CommandListImmediate)
	{
		// Dispatch a compute shader pass
		FComputeShaderUtils::Dispatch(CommandListImmediate, *NoiseComputeShader, NoisePassParameters, GroupCount);

		FComputeShaderUtils::Dispatch(CommandListImmediate, *ConvertComputeShader, ConvertPassParameters, GroupCount);
	});
}

