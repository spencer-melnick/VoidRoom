// Copyright 2019 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NoiseGenerator.h"

#include "NoiseGeneratorActor.generated.h"

UCLASS()
class OCEANSIM_API ANoiseGeneratorActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ANoiseGeneratorActor();

protected:
	virtual void BeginPlay() override;

private:
	FNoiseGenerator NoiseGenerator;

};
