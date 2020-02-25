#pragma once

#include "CoreMinimal.h"

#include "Engine/TextureRenderTarget2D.h"
#include "RHI.h"

class FNoiseGenerator
{
public:
	void AllocateResources(FIntPoint BufferSize);
	void Execute();

private:
	FStructuredBufferRHIRef NoiseBuffer = nullptr;
	FUnorderedAccessViewRHIRef NoiseBufferUAV = nullptr;
	FShaderResourceViewRHIRef NoiseBufferSRV = nullptr;
	FTexture2DRHIRef OutputTexture = nullptr;
	FUnorderedAccessViewRHIRef OutputTextureUAV = nullptr;
	FIntPoint NoiseBufferSize;
	FCriticalSection Mutex;
	bool bIsExecutionComplete = false;
};
