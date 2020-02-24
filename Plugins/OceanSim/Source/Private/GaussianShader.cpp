#include "GaussianShader.h"

#include "GaussianShaderDeclaration.h"

#include "Engine/Texture2D.h"
#include "Containers/DynamicRHIResourceArray.h"
#include "PixelShaderUtils.h"

void FGaussianShader::AllocateResources(FIntPoint BufferSize)
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
}


void FGaussianShader::Execute()
{
	// Set up compute shader parameters
	FGaussianShaderDeclaration::FParameters PassParameters;
	PassParameters.NoiseBuffer = NoiseBufferUAV;
	PassParameters.NoiseBufferSize = NoiseBufferSize;

	// Retrieve shader from global shader map
	TShaderMapRef<FGaussianShaderDeclaration> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

	// Calculate group count
	FIntVector GroupCount(FMath::DivideAndRoundUp(NoiseBufferSize.X, NUM_THREADS_PER_GROUP_DIMENSION),
		FMath::DivideAndRoundUp(NoiseBufferSize.Y, NUM_THREADS_PER_GROUP_DIMENSION),
		1);

	// Reset execution status
	Mutex.Lock();
	bIsExecutionComplete = false;
	Mutex.Unlock();

	UE_LOG(LogTemp, Display, TEXT("Dispatching Gaussian noise compute request"))

	// Execute deferred in the rendering thread
	ENQUEUE_RENDER_COMMAND(SceneDrawCompletion)([=](FRHICommandListImmediate& CommandListImmediate)
	{
		// Dispatch a compute shader pass
		FComputeShaderUtils::Dispatch(CommandListImmediate, *ComputeShader, PassParameters, GroupCount);

		// Notify the main thread that the computation is complete
		Mutex.Lock();
		bIsExecutionComplete = true;
		Mutex.Unlock();

		UE_LOG(LogTemp, Display, TEXT("Gaussian noise compute request completed"))
	});
}

