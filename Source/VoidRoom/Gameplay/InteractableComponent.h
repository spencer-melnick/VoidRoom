// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractableComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VOIDROOM_API UInteractableComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInteractableComponent();

	const unsigned char StencilOutlineValue = 2;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Used when a controller looks at or away from this object
	virtual void OnBecomeFocus();
	virtual void OnEndFocus();

	// Used when a pawn interacts with the object
	// OnGrabbed() should return whether or not to hold the object after the interaction
	virtual bool OnGrabbed();
	virtual void OnDropped();

private:
	void ShowHighlight();
	void HideHighlight();
};
