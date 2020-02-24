// Copyright 2019 Spencer Melnick


#include "NoiseGenerator.h"

// Sets default values
ANoiseGenerator::ANoiseGenerator()
{
}

// Called when the game starts or when spawned
void ANoiseGenerator::BeginPlay()
{
	Super::BeginPlay();

	GaussianShader.Execute(FIntPoint(128, 128));
}
