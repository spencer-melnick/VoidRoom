// Copyright 2019 Spencer Melnick


#include "InventoryGridWidget.h"

#include "../VoidRoom.h"
#include "../Controller/VDPlayerController.h"
#include "../Player/VDPlayerState.h"
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

void UInventoryGridWidget::NativeOnInitialized()
{
	// Check for valid player controller and player state
	AVDPlayerController* PlayerController = Cast<AVDPlayerController>(GetOwningPlayer());

	if (PlayerController == nullptr)
	{
		UE_LOG(LogVD, Error, TEXT("%s cannot get a valid VDPlayerController"), *GetNameSafe(this));
		return;
	}


	PlayerState = PlayerController->GetPlayerState<AVDPlayerState>();

	if (PlayerState == nullptr)
	{
		UE_LOG(LogVD, Error, TEXT("%s cannot get a valid VDPlayerState"), *GetNameSafe(this));
		return;
	}
}



void UInventoryGridWidget::UpdateDisplay()
{
	if (NameText != nullptr && DescriptionText != nullptr)
	{
		if (GetIsShowingActiveSlot())
		{
			UInventoryObject* Object = PlayerState->Inventory[ActiveInventoryIndex].Object;

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

bool UInventoryGridWidget::GetIsShowingActiveSlot() const
{
	if (PlayerState == nullptr)
	{
		return false;
	}
	
	return bShouldDisplayActiveItem && ActiveInventoryIndex < PlayerState->Inventory.Num();
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
	if (GetIsShowingActiveSlot())
	{
		UInventoryObject* Object = PlayerState->Inventory[ActiveInventoryIndex].Object;

		if (Object->ToolComponent != nullptr)
		{
			// Possibly move this logic out to the player controller instead
			AVDCharacter* Character = Cast<AVDCharacter>(GetOwningPlayerPawn());
			Character->EquipTool(Object->ToolComponent);
		}
	}
}


