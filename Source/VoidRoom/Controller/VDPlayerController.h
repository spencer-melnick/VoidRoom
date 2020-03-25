// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "../UI/RoleDisplay.h"
#include "VDPlayerController.generated.h"


/**
 * 
 */
UCLASS()
class VOIDROOM_API AVDPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// Override public interface
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="UI")
	TSubclassOf<class UUserWidget> UIClass;
	
protected:
	// VD protected interface

	void MoveForward(float Scale);
	void MoveRight(float Scale);

	void LookUp(float Scale);
	void LookRight(float Scale);

	void Jump();
	void EndJump();

	void Crouch();
	void UnCrouch();

	void Interact();
	void Climb();

private:

	UUserWidget* UIWidget;
};
