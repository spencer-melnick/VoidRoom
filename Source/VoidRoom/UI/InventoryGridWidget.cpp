// Copyright 2019 Spencer Melnick


#include "InventoryGridWidget.h"

#include "VoidRoom/VoidRoom.h"
#include "VoidRoom/Controller/VDPlayerController.h"
#include "VoidRoom/Player/VDPlayerState.h"
#include "VoidRoom/Character/VDCharacter.h"


void UInventoryGridWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	// Build the children grid widgets
	RecreateGrid();

	// Attach main buttons
	if (EquipButton != nullptr)
	{
		EquipButton->OnClicked.AddUniqueDynamic(this, &UInventoryGridWidget::OnEquipClicked);
	}
}

void UInventoryGridWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	// UpdateDisplay();
}



void UInventoryGridWidget::UpdateDisplay()
{
	if (NameText != nullptr && DescriptionText != nullptr)
	{
		FInventorySlot* ActiveSlot = GetActiveSlot();
		
		if (ActiveSlot != nullptr)
		{
			UInventoryObject* Object = ActiveSlot->Object;

			NameText->SetText(Object->Name);
			DescriptionText->SetText(Object->Description);
			EquipButton->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			NameText->SetText(FText());
			DescriptionText->SetText(FText());
			EquipButton->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UInventoryGridWidget::SetActiveSlot(int32 NewInventoryIndex)
{
	ActiveInventoryIndex = NewInventoryIndex;
	bShouldDisplayActiveItem = true;

	UpdateDisplay();
}

void UInventoryGridWidget::HideActiveSlot()
{
	bShouldDisplayActiveItem = false;

	UpdateDisplay();
}

FInventorySlot* UInventoryGridWidget::GetActiveSlot() const
{
	if (!bShouldDisplayActiveItem)
	{
		return nullptr;
	}

	APlayerController* PlayerController = GetOwningPlayer();

	if (PlayerController == nullptr)
	{
		UE_LOG(LogVD, Error, TEXT("%s is not associated with a player controller"), *GetNameSafe(this));
		return nullptr;
	}

	AVDPlayerState* PlayerState = PlayerController->GetPlayerState<AVDPlayerState>();

	if (PlayerState == nullptr)
	{
		return nullptr;
	}

	auto& Inventory = PlayerState->Inventory;

	if (ActiveInventoryIndex >= Inventory.Num())
	{
		return nullptr;
	}

	return &Inventory[ActiveInventoryIndex];
}




void UInventoryGridWidget::RecreateGrid()
{
	HideActiveSlot();
	
	if (InventoryPanel != nullptr && ItemWidgetClass != nullptr)
	{
		InventoryPanel->ClearChildren();
		ItemWidgets.Empty();

		uint32 InventoryIndex = 0;
		
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
				InventoryItemWidget->SetInventorySlot(InventoryIndex++);
			}
		}
	}
}


void UInventoryGridWidget::OnEquipClicked()
{
	FInventorySlot* ActiveSlot = GetActiveSlot();
	
	if (ActiveSlot != nullptr)
	{
		UInventoryObject* Object = ActiveSlot->Object;

		if (Object->ToolComponent != nullptr)
		{
			// Possibly move this logic out to the player controller instead
			AVDCharacter* Character = Cast<AVDCharacter>(GetOwningPlayerPawn());
			Character->EquipTool(Object->ToolComponent);
		}
	}
}


