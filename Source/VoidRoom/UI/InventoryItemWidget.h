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
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// Public functions
	void SetInventorySlot(int32 SlotIndex);
	void SetOwner(class UInventoryGridWidget* NewOwner);
	struct FInventorySlot* GetAssociatedSlot() const;

	UFUNCTION()
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

	int32 InventoryIndex;
	class UInventoryGridWidget* Owner = nullptr;
};
