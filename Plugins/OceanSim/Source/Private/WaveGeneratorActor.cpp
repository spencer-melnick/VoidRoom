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

	FIntPoint RenderTargetSize(RenderTarget->GetSurfaceHeight(), RenderTarget->GetSurfaceWidth());

	if (RenderTarget->GetFormat() != EPixelFormat::PF_A32B32G32R32F &&
		RenderTarget->GetFormat() != EPixelFormat::PF_FloatRGBA)
	{
		UE_LOG(LogTemp, Error, TEXT("Wave Generator render target must be either in float16 or float32 RGBA format"));
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
	
	WaveGenerator.Initialize(RenderTargetSize.X, RenderTarget);
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
		PropertyName == GET_MEMBER_NAME_CHECKED(AWaveGeneratorActor, WindDirection))
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

	WaveGenerator.SetParameters(Parameters);
}

