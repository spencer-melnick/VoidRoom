#include "GaussianShader.h"

#include "GaussianShaderDeclaration.h"

#include "Engine/Texture2D.h"
#include "Containers/DynamicRHIResourceArray.h"
#include "PixelShaderUtils.h"

void FGaussianShader::Execute(FIntPoint BufferSize)
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
	
	FRHIResourceCreateInfo CreateInfo;

	// Create input texture
	UniformNoiseTexture = RHICreateTexture2D(BufferSize.X, BufferSize.Y, PF_A32B32G32R32F, 1, 1,
		TexCreate_ShaderResource, CreateInfo);
	UniformNoiseTextureSRV = RHICreateShaderResourceView(UniformNoiseTexture, 0);

	// Create output texture array
	GaussianNoiseTexture = RHICreateTexture2DArray(BufferSize.X, BufferSize.Y, 4, PF_R32_FLOAT, 1, 1,
		TexCreate_ShaderResource | TexCreate_UAV, CreateInfo);
	GaussianNoiseTextureUAV = RHICreateUnorderedAccessView(GaussianNoiseTexture);

	// Set up compute shader parameters
	FGaussianShaderDeclaration::FParameters PassParameters;
	PassParameters.UniformNoiseTexture = UniformNoiseTextureSRV;
	PassParameters.GaussianNoiseTexture = GaussianNoiseTextureUAV;

	// Retrieve shader from global shader map
	TShaderMapRef<FGaussianShaderDeclaration> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

	// Calculate group count
	FIntVector GroupCount(FMath::DivideAndRoundUp(BufferSize.X, NUM_THREADS_PER_GROUP_DIMENSION),
		FMath::DivideAndRoundUp(BufferSize.Y, NUM_THREADS_PER_GROUP_DIMENSION),
		1);

	// Reset execution status
	Mutex.Lock();
	bIsExecutionComplete = false;
	Mutex.Unlock();

	UE_LOG(LogTemp, Display, TEXT("Dispatching Gaussian noise compute request"))
	
	// Execute deferred in the rendering thread
	ENQUEUE_RENDER_COMMAND(SceneDrawCompletion)([=](FRHICommandListImmediate& CommandListImmediate)
	{
		// Lock the texture and copy the noise data
		uint32 Stride = 0;
		FVector4* RawData = static_cast<FVector4*>(RHILockTexture2D(UniformNoiseTexture, 0, EResourceLockMode::RLM_WriteOnly, Stride, false));
		FMemory::Memcpy(RawData, NoiseData.GetData(), NumElements * sizeof(FVector4));
		RHIUnlockTexture2D(UniformNoiseTexture, 0, false);

		// Dispatch a compute shader pass
		FComputeShaderUtils::Dispatch(CommandListImmediate, *ComputeShader, PassParameters, GroupCount);

		// Notify the main thread that the computation is complete
		Mutex.Lock();
		bIsExecutionComplete = true;
		Mutex.Unlock();

		UE_LOG(LogTemp, Display, TEXT("Gaussian noise compute request completed"))
	});
}


