#include "GaussianShader.h"

#include "GaussianShaderDeclaration.h"

#include "Engine/Texture2D.h"
#include "Containers/DynamicRHIResourceArray.h"
#include "PixelShaderUtils.h"

FGaussianShader::FGaussianShader(FIntPoint BufferSize) :
	BufferSize(BufferSize)
{
}

void FGaussianShader::Execute()
{
	unsigned int NumElements = BufferSize.X * BufferSize.Y;

	// Allocate a large array for noise data
	TArray<FVector4> NoiseData;
	NoiseData.Init(FVector4(), NumElements);

	// Generate some random uniform noise
	for (unsigned int i = 0; i < NumElements; i++)
	{
		NoiseData[i] = FVector4(FMath::FRandRange(0, 1), FMath::FRandRange(0, 1), FMath::FRandRange(0, 1), FMath::FRandRange(0, 1));
	}
	
	// Execute deferred in the rendering thread
	ENQUEUE_RENDER_COMMAND(SceneDrawCompletion)([=](FRHICommandListImmediate& CommandListImmediate)
	{
		FRHIResourceCreateInfo CreateInfo;

		// Create a new texture
		Buffer = RHICreateTexture2D(BufferSize.X, BufferSize.Y, PF_A32B32G32R32F, 1, 1,
			TexCreate_CPUWritable | TexCreate_ShaderResource | TexCreate_UAV, CreateInfo);
		BufferUAV = RHICreateUnorderedAccessView(Buffer);

		// Lock the texture and copy the noise data
		uint32 Stride = 0;
		FVector4* RawData = static_cast<FVector4*>(RHILockTexture2D(Buffer, 0, EResourceLockMode::RLM_WriteOnly, Stride, false));
		FMemory::Memcpy(RawData, NoiseData.GetData(), NumElements * sizeof(FVector4));
		RHIUnlockTexture2D(Buffer, 0, false);

		// Set up compute shader parameters
		FGaussianShaderDeclaration::FParameters PassParameters;
		PassParameters.Buffer = BufferUAV;

		// Retrieve shader from global shader map
		TShaderMapRef<FGaussianShaderDeclaration> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

		// Calculate group count
		FIntVector GroupCount(FMath::DivideAndRoundUp(BufferSize.X, NUM_THREADS_PER_GROUP_DIMENSION),
			FMath::DivideAndRoundUp(BufferSize.Y, NUM_THREADS_PER_GROUP_DIMENSION),
			1);

		// Dispatch a compute shader pass
		FComputeShaderUtils::Dispatch(CommandListImmediate, *ComputeShader, PassParameters, GroupCount);
	});
}


