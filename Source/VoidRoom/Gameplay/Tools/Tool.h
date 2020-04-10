// Copyright 2019 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Tool.generated.h"

UCLASS(Abstract, Blueprintable)
class VOIDROOM_API UTool: public USceneComponent
{
	GENERATED_BODY()

public:
	// Interface functions

	// Due to the character calling Tool::OnUse from a multicast RPC, OnUse will run
	// on all players' machines. Make sure to check for local control if necessary
	virtual void OnUse(class AVDCharacter* Character);

	
	// Accessors
	
	TSubclassOf<UAnimInstance> GetCharacterViewAnimationBlueprint() const;
	FName GetAttachmentSocket() const;

private:
	UPROPERTY(EditAnywhere, Category = Tool)
	TSubclassOf<UAnimInstance> CharacterViewAnimationBlueprint;
	// Controls the animation for the character's first person view

	UPROPERTY(EditAnywhere, Category = Tool)
	FName AttachmentSocket = NAME_None;
	// Where, if anywhere, this component is attached to the character
};
