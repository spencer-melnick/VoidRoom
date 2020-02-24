#pragma once

#include "CoreMinimal.h"
#include "RHI.h"

class FGaussianShader
{
public:
	FGaussianShader(FIntPoint BufferSize);

private:
	FIntPoint BufferSize;
	FTexture2DRHIRef Buffer = nullptr;
	FUnorderedAccessViewRHIRef BufferUAV = nullptr;

	void Execute();
};
