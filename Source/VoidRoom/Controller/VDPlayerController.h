// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "VDPlayerController.generated.h"


/**
 * 
 */
UCLASS()
class VOIDROOM_API AVDPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// State class
	enum class EControlState
	{
		GameControl,
		MenuControl,
		InventoryControl
	};

	
	// Engine overrides
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;


	// Public functions
	void SetControlState(EControlState NewControlState);


	// Public properties
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> InventoryGridClass;
	
protected:
	// VD protected interface

	void MoveForward(float Scale);
	void MoveRight(float Scale);

	void Jump();
	void EndJump();

	void Crouch();
	void UnCrouch();

	void Interact();
	void Climb();

	void PrimaryAction();

	void ToggleInventory();

private:
	void CreateUIWidgets();
	void HideUIWidgets();


	// Private variables
	EControlState ControlState;
	
	// UI Widgets
	UUserWidget* UIWidget;
	UUserWidget* InventoryGridWidget;
};
