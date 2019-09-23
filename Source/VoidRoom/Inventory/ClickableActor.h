// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Inventory/ObtainableComponent.h"
#include "ClickableActor.generated.h"

UCLASS()
class VOIDROOM_API AClickableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AClickableActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void NotifyActorOnClicked(FKey ButtonPressed) override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
