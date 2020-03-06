// Copyright 2019 Spencer Melnick


#include "WaveGeneratorActor.h"

#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"

// Sets default values
AWaveGeneratorActor::AWaveGeneratorActor()
{
}

// Called when the game starts or when spawned
void AWaveGeneratorActor::BeginPlay()
{
	Super::BeginPlay();
	UpdateGeneratorParameters();

	FIntPoint RenderTargetSize(DisplacementRenderTarget->GetSurfaceHeight(), DisplacementRenderTarget->GetSurfaceWidth());

	if (SlopeRenderTarget->GetSurfaceWidth() != RenderTargetSize.X || SlopeRenderTarget->GetSurfaceHeight() != RenderTargetSize.Y)
	{
		UE_LOG(LogTemp, Error, TEXT("Wave Generator displacement and slope render targets must be the same size"));
		return;
	}

	if (DisplacementRenderTarget->GetFormat() != EPixelFormat::PF_A32B32G32R32F &&
		DisplacementRenderTarget->GetFormat() != EPixelFormat::PF_FloatRGBA)
	{
		UE_LOG(LogTemp, Error, TEXT("Wave Generator displacement render target must be either in float16 RGB or float32 RGBA format"));
		return;
	}

	if (SlopeRenderTarget->GetFormat() != EPixelFormat::PF_G16R16F &&
		SlopeRenderTarget->GetFormat() != EPixelFormat::PF_G32R32F)
	{
		UE_LOG(LogTemp, Error, TEXT("Wave Generator slope render target must be either in float16 GR or float32 GR format"));
		return;
	}

	if (RenderTargetSize.X != RenderTargetSize.Y)
	{
		UE_LOG(LogTemp, Error, TEXT("Wave Generator render target must be square"));
		return;
	}

	if (!FMath::IsPowerOfTwo(RenderTargetSize.X))
	{
		UE_LOG(LogTemp, Error, TEXT("Wave Generator render target must have a power of two size"));
		return;
	}
	
	WaveGenerator.Initialize(RenderTargetSize.X, DisplacementRenderTarget, SlopeRenderTarget);
	WaveGenerator.BeginRendering();
}

void AWaveGeneratorActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	WaveGenerator.StopRendering();
}

#ifdef WITH_EDITOR
void AWaveGeneratorActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(AWaveGeneratorActor, Amplitude) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(AWaveGeneratorActor, PatchLength) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(AWaveGeneratorActor, Gravity) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(AWaveGeneratorActor, WindSpeed) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(AWaveGeneratorActor, WindDirection) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(AWaveGeneratorActor, FoamLambda))
	{
		UpdateGeneratorParameters();
	}
}
#endif

void AWaveGeneratorActor::UpdateGeneratorParameters()
{
	FWaveGenerator::FGenerationParameters Parameters;
	Parameters.Amplitude = Amplitude;
	Parameters.PatchLength = PatchLength;
	Parameters.Gravity = Gravity;
	Parameters.WindSpeed = WindSpeed;
	Parameters.WindDirection = WindDirection;
	Parameters.FoamLambda = FoamLambda;

	WaveGenerator.SetParameters(Parameters);
}

