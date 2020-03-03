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

	WaveGenerator.Initialize(256, RenderTarget);
	WaveGenerator.BeginRendering();
}

void AWaveGeneratorActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	WaveGenerator.StopRendering();
}

