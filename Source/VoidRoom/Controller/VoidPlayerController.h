// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "VoidPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class VOIDROOM_API AVoidPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AVoidPlayerController();

private:
	void MoveForward(float Scale);
	void MoveRight(float Scale);

	void LookUp(float Scale);
	void LookRight(float Scale);

	void Crouch();
	void UnCrouch();
	
public:
	virtual void SetupInputComponent() override;
};
