#pragma once

#include "CoreMinimal.h"

#include "RHI.h"

class FWaveGenerator
{
public:
	void Initialize(FIntPoint Dimensions);
	void GenerateGaussianNoise();

private:
	bool bHasGaussianNoise = false;
	FIntPoint BufferSize;
	FTexture2DRHIRef GaussianNoiseTexture;
	FShaderResourceViewRHIRef GaussianNoiseTextureSRV;
};
