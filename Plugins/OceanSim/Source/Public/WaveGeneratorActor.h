// Copyright 2019 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaveGenerator.h"

#include "WaveGeneratorActor.generated.h"

UCLASS()
class OCEANSIM_API AWaveGeneratorActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AWaveGeneratorActor();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	FWaveGenerator WaveGenerator;

};
