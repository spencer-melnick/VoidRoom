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
	struct FButterflyOperation
	{
		FButterflyOperation(uint32 IndexA, uint32 IndexB, FVector2D Twiddle) :
			IndexA(IndexA), IndexB(IndexB), Twiddle(Twiddle)
		{};
		
		uint32 IndexA;
		uint32 IndexB;
		FVector2D Twiddle;
	};
	
	void OnRender(FRHICommandListImmediate& RHICmdList, class FSceneRenderTargets& SceneContext);
	void GenerateGaussianNoise();
	void CalculateButterflyOperations();
	
	static FVector2D ImaginaryExponent(float Theta);
	static FVector2D ComplexMultiply(FVector2D A, FVector2D B);


	// Shader variables
	bool bHasGaussianNoise = false;
	bool bAreParametersUpToDate = false;
	FIntPoint BufferSize;
	float StartTime = 0;
	
	FTexture2DRHIRef GaussianNoiseTexture;
	FShaderResourceViewRHIRef GaussianNoiseTextureSRV;
	
	FTexture2DRHIRef InitialComponentsTexture;
	FUnorderedAccessViewRHIRef InitialComponentsTextureUAV;
	FShaderResourceViewRHIRef InitialComponentsTextureSRV;

	FStructuredBufferRHIRef ButterflyBuffer;
	FShaderResourceViewRHIRef ButterflyBufferSRV;
	

	// Rendering hooks
	FDelegateHandle ResolvedSceneColorHandle;
};
