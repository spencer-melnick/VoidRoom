#pragma once

#include "CoreMinimal.h"

#include "RHI.h"

class FWaveGenerator
{
public:
	~FWaveGenerator();
	
	void Initialize(FIntPoint Dimensions);
	void BeginRendering();
	void StopRendering();

private:
	void OnRender(FRHICommandListImmediate& RHICmdList, class FSceneRenderTargets& SceneContext);
	void GenerateGaussianNoise();


	// Shader variables
	bool bHasGaussianNoise = false;
	bool bAreParametersUpToDate = false;
	FIntPoint BufferSize;
	float StartTime;
	
	FTexture2DRHIRef GaussianNoiseTexture;
	FShaderResourceViewRHIRef GaussianNoiseTextureSRV;
	
	FTexture2DRHIRef InitialComponentsTexture;
	FUnorderedAccessViewRHIRef InitialComponentsTextureUAV;
	FShaderResourceViewRHIRef InitialComponentsTextureSRV;
	

	// Rendering hooks
	FDelegateHandle ResolvedSceneColorHandle;
};
