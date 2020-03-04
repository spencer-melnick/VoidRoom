#pragma once

#include "CoreMinimal.h"

#include "RHI.h"
#include "Engine/TextureRenderTarget2D.h"
#include "RenderGraphBuilder.h"

#include "WaveGenerator.h"

class FWaveGenerator
{
public:
	enum class EFFTDirection
	{
		FFT_Horizontal,
		FFT_Vertical
	};
	
	struct FGenerationParameters
	{
		float Amplitude = 1;
		float PatchLength = 1000;
		float Gravity = 9.81;
		float WindSpeed = 40;
		FVector2D WindDirection = FVector2D(1, 0);
	};
	
	~FWaveGenerator();
	
	void Initialize(uint32 LengthInPoints, UTextureRenderTarget2D* HeightTarget, UTextureRenderTarget2D* SlopeTarget);
	void BeginRendering();
	void StopRendering();
	void SetParameters(FGenerationParameters NewParameters);

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

	template<EFFTDirection Direction>
	void DoFFT(FRDGBuilder& GraphBuilder, FRDGBufferUAVRef DataSet);
	void DoFFT2(FRDGBuilder& GraphBuilder, FRDGBufferUAVRef DataSet);
	
	static FVector2D ImaginaryExponent(float Theta);
	static FVector2D ComplexMultiply(FVector2D A, FVector2D B);
	static uint32 BitReverse(uint32 Value, uint32 NumBits);
	static TRefCountPtr<IPooledRenderTarget> GetPooledRenderTarget(UTextureRenderTarget2D* RenderTarget);


	// Shader variables
	FCriticalSection ParameterLock;
	FGenerationParameters GenerationParameters;
	bool bHasGaussianNoise = false;
	bool bAreParametersUpToDate = false;
	uint32 Length = 0;
	uint32 NumSteps = 0;
	FIntPoint BufferSize;
	float StartTime = 0;

	
	// Textures / Buffers
	UTextureRenderTarget2D* DisplacementRenderTarget = nullptr;
	UTextureRenderTarget2D* SlopeRenderTarget = nullptr;

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
