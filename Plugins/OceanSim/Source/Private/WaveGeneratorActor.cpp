// Copyright 2019 Spencer Melnick


#include "WaveGeneratorActor.h"

// Sets default values
AWaveGeneratorActor::AWaveGeneratorActor()
{
}

// Called when the game starts or when spawned
void AWaveGeneratorActor::BeginPlay()
{
	Super::BeginPlay();

	WaveGenerator.Initialize(FIntPoint(256, 256));
	WaveGenerator.BeginRendering();
}

void AWaveGeneratorActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	WaveGenerator.StopRendering();
}

