// Copyright 2019 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GaussianShader.h"

#include "NoiseGenerator.generated.h"

UCLASS()
class OCEANSIM_API ANoiseGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	ANoiseGenerator();

protected:
	virtual void BeginPlay() override;

private:
	FGaussianShader GaussianShader;

};
