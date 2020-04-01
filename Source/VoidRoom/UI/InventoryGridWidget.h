// Copyright 2019 Spencer Melnick

#pragma once

// Default includes
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

// Component includes
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

// Game includes
#include "InventoryItemWidget.h"

// Generated header
#include "InventoryGridWidget.generated.h"

/**
 * 
 */
UCLASS()
class VOIDROOM_API UInventoryGridWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Engine overrides
	void SynchronizeProperties() override;
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;


	// Public functions
	void UpdateDisplay();
	void SetActiveSlot(FInventorySlot Slot);
	void ClearActiveSlot();
	
	
	// Children widgets
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* DescriptionText = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* NameText = nullptr;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UUniformGridPanel* InventoryPanel = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UInventoryItemWidget> ItemWidgetClass = nullptr;


	// Public properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
	int Rows = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
	int Columns = 0;

private:
	void RecreateGrid();

	TArray<UInventoryItemWidget*> ItemWidgets;
};
