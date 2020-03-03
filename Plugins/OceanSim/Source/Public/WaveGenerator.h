#pragma once

#include "CoreMinimal.h"

#include "RHI.h"
#include "Engine/TextureRenderTarget2D.h"

#include "WaveGenerator.h"

class FWaveGenerator
{
public:
	~FWaveGenerator();
	
	void Initialize(uint32 LengthInPoints, UTextureRenderTarget2D* Target);
	void BeginRendering();
	void StopRendering();

	UTextureRenderTarget2D* GetRenderTarget() const;

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

	struct FSwapIndex
	{
		uint32 IndexA;
		uint32 IndexB;
	};
	
	void OnRender(FRHICommandListImmediate& RHICmdList, class FSceneRenderTargets& SceneContext);
	void GenerateGaussianNoise();
	void GenerateBitReversal();
	void CalculateButterflyOperations();
	
	static FVector2D ImaginaryExponent(float Theta);
	static FVector2D ComplexMultiply(FVector2D A, FVector2D B);
	static uint32 BitReverse(uint32 Value, uint32 NumBits);


	// Shader variables
	bool bHasGaussianNoise = false;
	bool bAreParametersUpToDate = false;
	uint32 Length = 0;
	uint32 NumSteps = 0;
	FIntPoint BufferSize;
	float StartTime = 0;

	
	// Textures / Buffers
	UTextureRenderTarget2D* RenderTarget;

	FTexture2DRHIRef GaussianNoiseTexture;
	FShaderResourceViewRHIRef GaussianNoiseTextureSRV;
	
	FTexture2DRHIRef InitialComponentsTexture;
	FUnorderedAccessViewRHIRef InitialComponentsTextureUAV;
	FShaderResourceViewRHIRef InitialComponentsTextureSRV;

	FStructuredBufferRHIRef BitReverseBuffer;
	FShaderResourceViewRHIRef BitReverseBufferSRV;

	FStructuredBufferRHIRef ButterflyBuffer;
	FShaderResourceViewRHIRef ButterflyBufferSRV;
	

	// Rendering hooks
	FDelegateHandle ResolvedSceneColorHandle;
};
