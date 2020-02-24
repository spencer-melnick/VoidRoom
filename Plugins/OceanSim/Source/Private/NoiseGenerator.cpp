#include "NoiseGenerator.h"

#include "BoxMullerShader.h"

#include "Engine/Texture2D.h"
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
}


void FNoiseGenerator::Execute()
{
	// Set up compute shader parameters
	FBoxMullerShader::FParameters PassParameters;
	PassParameters.NoiseBuffer = NoiseBufferUAV;
	PassParameters.NoiseBufferSize = NoiseBufferSize;

	// Retrieve shader from global shader map
	TShaderMapRef<FBoxMullerShader> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

	// Calculate group count
	FIntVector GroupCount(FMath::DivideAndRoundUp(NoiseBufferSize.X, NUM_THREADS_PER_GROUP_DIMENSION),
		FMath::DivideAndRoundUp(NoiseBufferSize.Y, NUM_THREADS_PER_GROUP_DIMENSION),
		1);

	// Reset execution status
	Mutex.Lock();
	bIsExecutionComplete = false;
	Mutex.Unlock();

	uint32 NumElements = NoiseBufferSize.X * NoiseBufferSize.Y;

	UE_LOG(LogTemp, Display, TEXT("Dispatching Gaussian noise compute request"))

	// Execute deferred in the rendering thread
	ENQUEUE_RENDER_COMMAND(SceneDrawCompletion)([=](FRHICommandListImmediate& CommandListImmediate)
	{
		// Dispatch a compute shader pass
		FComputeShaderUtils::Dispatch(CommandListImmediate, *ComputeShader, PassParameters, GroupCount);

		FVector4* ReadData = static_cast<FVector4*>(RHILockStructuredBuffer(NoiseBuffer, 0, sizeof(FVector4) * NumElements, EResourceLockMode::RLM_ReadOnly));
		UE_LOG(LogTemp, Display, TEXT("Sample[0]: %f, %f"), ReadData[0].X, ReadData[0].Y)
		RHIUnlockStructuredBuffer(NoiseBuffer);

		// Notify the main thread that the computation is complete
		Mutex.Lock();
		bIsExecutionComplete = true;
		Mutex.Unlock();

		UE_LOG(LogTemp, Display, TEXT("Gaussian noise compute request completed"))
	});
}

