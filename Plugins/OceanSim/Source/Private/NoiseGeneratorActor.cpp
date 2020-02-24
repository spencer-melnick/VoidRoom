// Copyright 2019 Spencer Melnick


#include "NoiseGeneratorActor.h"

// Sets default values
ANoiseGeneratorActor::ANoiseGeneratorActor()
{
}

// Called when the game starts or when spawned
void ANoiseGeneratorActor::BeginPlay()
{
	Super::BeginPlay();

	NoiseGenerator.AllocateResources(FIntPoint(128, 128));
	NoiseGenerator.Execute();
}
