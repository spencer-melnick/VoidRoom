// Copyright 2019 Spencer Melnick


#include "InventoryGridWidget.h"

#include "../VoidRoom.h"
#include "../Controller/VDPlayerController.h"
#include "../Player/VDPlayerState.h"


void UInventoryGridWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	
	RecreateGrid();
}

void UInventoryGridWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	UpdateDisplay();
}


void UInventoryGridWidget::UpdateDisplay()
{
	// Check for valid player controller and player state
	AVDPlayerController* PlayerController = Cast<AVDPlayerController>(GetOwningPlayer());

	if (PlayerController == nullptr)
	{
		UE_LOG(LogVD, Error, TEXT("%s cannot get a valid VDPlayerController"), *GetNameSafe(this));
		return;
	}

	AVDPlayerState* PlayerState = PlayerController->GetPlayerState<AVDPlayerState>();

	if (PlayerState == nullptr)
	{
		UE_LOG(LogVD, Error, TEXT("%s cannot get a valid VDPlayerState"), *GetNameSafe(this));
		return;
	}

	// Update the properties of all the item widgets
	for (int i = 0; i < ItemWidgets.Num(); i++)
	{
		if (i < PlayerState->Inventory.Num())
		{
			ItemWidgets[i]->SetInventorySlot(PlayerState->Inventory[i]);
		}
		else
		{
			ItemWidgets[i]->SetEmpty();
		}
	}
}

void UInventoryGridWidget::SetActiveSlot(FInventorySlot InventorySlot)
{
	if (NameText != nullptr && DescriptionText != nullptr && InventorySlot.Object != nullptr)
	{
		NameText->SetText(InventorySlot.Object->Name);
		DescriptionText->SetText(InventorySlot.Object->Description);
	}
}

void UInventoryGridWidget::ClearActiveSlot()
{
	if (NameText != nullptr && DescriptionText != nullptr)
	{
		NameText->SetText(FText());
		DescriptionText->SetText(FText());
	}
}




void UInventoryGridWidget::RecreateGrid()
{
	ClearActiveSlot();
	
	if (InventoryPanel != nullptr && ItemWidgetClass != nullptr)
	{
		InventoryPanel->ClearChildren();
		ItemWidgets.Empty();

		for (int Row = 0; Row < Rows; Row++)
		{
			for (int Column = 0; Column < Columns; Column++)
			{
				// Spawn an inventory item widget and add it to the grid
				UInventoryItemWidget* InventoryItemWidget = Cast<UInventoryItemWidget>(CreateWidget(this, ItemWidgetClass));
				UUniformGridSlot* ItemGridSlot = InventoryPanel->AddChildToUniformGrid(InventoryItemWidget, Row, Column);
				
				ItemGridSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
				ItemGridSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);

				ItemWidgets.Add(InventoryItemWidget);
				InventoryItemWidget->SetOwner(this);
			}
		}
	}
}


