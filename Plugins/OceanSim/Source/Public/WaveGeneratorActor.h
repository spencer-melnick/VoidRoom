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

	#ifdef WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	#endif
	
	UPROPERTY(Category = "Wave Generation", BlueprintReadWrite, EditAnywhere)
	UTextureRenderTarget2D* RenderTarget;

	UPROPERTY(Category = "Wave Generation", BlueprintReadWrite, EditAnywhere)
	float Amplitude = 4;

	UPROPERTY(Category = "Wave Generation", BlueprintReadWrite, EditAnywhere)
	float PatchLength = 1000;

	UPROPERTY(Category = "Wave Generation", BlueprintReadWrite, EditAnywhere)
	float Gravity = 9.81;

	UPROPERTY(Category = "Wave Generation", BlueprintReadWrite, EditAnywhere)
	float WindSpeed = 40;

	UPROPERTY(Category = "Wave Generation", BlueprintReadWrite, EditAnywhere)
	FVector2D WindDirection = FVector2D(1, 0);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void UpdateGeneratorParameters();
	
	FWaveGenerator WaveGenerator;

};
