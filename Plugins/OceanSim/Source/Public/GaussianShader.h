#pragma once

#include "CoreMinimal.h"
#include "RHI.h"

class FGaussianShader
{
public:
	void AllocateResources(FIntPoint BufferSize);
	void Execute();

private:
	FStructuredBufferRHIRef NoiseBuffer = nullptr;
	FUnorderedAccessViewRHIRef NoiseBufferUAV = nullptr;
	FIntPoint NoiseBufferSize;
	FCriticalSection Mutex;
	bool bIsExecutionComplete = false;
};
