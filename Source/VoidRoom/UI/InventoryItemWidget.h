// Copyright 2019 Spencer Melnick

#pragma once

// Default includes
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

// Component includes
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Button.h"

// Game includes
#include "../Gameplay/Inventory/InventoryObject.h"
#include "VoidRoom/Player/VDPlayerState.h"

// Generated header
#include "InventoryItemWidget.generated.h"

/**
 * 
 */
UCLASS()
class VOIDROOM_API UInventoryItemWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Engine overrides
	void SynchronizeProperties() override;

	// Public functions
	void SetInventorySlot(FInventorySlot NewSlot);
	void SetEmpty();
	void SetOwner(class UInventoryGridWidget* NewOwner);

	void OnClicked();
	
	// Child widgets
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* DisplayText = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* DisplayImage = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* MainButton = nullptr;

private:
	void UpdateDisplay();

	bool bIsEmpty = true;
	FInventorySlot InventorySlot;
	class UInventoryGridWidget* Owner = nullptr;
};
