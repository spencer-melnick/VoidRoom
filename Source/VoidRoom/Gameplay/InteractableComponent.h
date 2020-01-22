// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "../Character/VDCharacter.h"

#include "InteractableComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VOIDROOM_API UInteractableComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInteractableComponent();

	// Used when a character looks at or away from this object
	virtual void OnFocused(AVDCharacter* Character);
	virtual void OnUnfocused(AVDCharacter* Character);

	// Networked interact
	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void MulticastInteract(AVDCharacter* Character);

protected:
	// Used when a character interacts with this object
	virtual void OnInteract(AVDCharacter* Character);

	// Hides or shows outline using post-process shader
	void ShowHighlight();
	void HideHighlight();
};
