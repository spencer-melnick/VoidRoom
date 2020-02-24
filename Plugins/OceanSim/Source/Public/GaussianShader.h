#pragma once

#include "CoreMinimal.h"
#include "RHI.h"

class FGaussianShader
{
public:
	void Execute(FIntPoint BufferSize);

private:
	FTexture2DArrayRHIRef GaussianNoiseTexture = nullptr;
	FUnorderedAccessViewRHIRef GaussianNoiseTextureUAV = nullptr;
	FTexture2DRHIRef UniformNoiseTexture = nullptr;
	FShaderResourceViewRHIRef UniformNoiseTextureSRV = nullptr;
	FCriticalSection Mutex;
	bool bIsExecutionComplete = false;
};
