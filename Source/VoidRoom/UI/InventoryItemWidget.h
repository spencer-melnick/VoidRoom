// Copyright 2019 Spencer Melnick

#pragma once

// Default includes
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

// Component includes
#include "Components/TextBlock.h"

// Game includes
#include "../Gameplay/Inventory/InventoryObject.h"

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
	void SetObject(FInventoryObject NewObject);
	void SetEmpty();
	
	// Child widgets
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* DisplayText = nullptr;

private:
	void UpdateDisplay();

	bool bIsEmpty = true;
	FInventoryObject Object;
};
