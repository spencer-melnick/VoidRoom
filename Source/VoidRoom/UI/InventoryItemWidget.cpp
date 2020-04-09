// Copyright 2019 Spencer Melnick


#include "InventoryItemWidget.h"

#include "VoidRoom/VoidRoom.h"
#include "InventoryGridWidget.h"

void UInventoryItemWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	
	UpdateDisplay();
}

void UInventoryItemWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	UpdateDisplay();
}

void UInventoryItemWidget::NativeOnInitialized()
{
	APlayerController* Player = GetOwningPlayer();

	if (Player != nullptr)
	{
		PlayerState = Player->GetPlayerState<AVDPlayerState>();
		
		if (PlayerState == nullptr)
		{
			UE_LOG(LogVD, Warning, TEXT("%s is bound to a player without a VDPlayerState"), *GetNameSafe(this));
		}
	}
}



void UInventoryItemWidget::SetInventorySlot(int32 NewInventoryIndex)
{
	InventoryIndex = NewInventoryIndex;

	UpdateDisplay();
}

void UInventoryItemWidget::SetOwner(UInventoryGridWidget* NewOwner)
{
	Owner = NewOwner;

	if (MainButton != nullptr)
	{
		MainButton->OnClicked.AddUniqueDynamic(this, &UInventoryItemWidget::OnClicked);
	}
}

bool UInventoryItemWidget::GetIsEmpty() const
{
	if (PlayerState == nullptr)
	{
		return true;
	}

	// Considered not empty if the index is less than the size of the inventory array
	return InventoryIndex >= PlayerState->Inventory.Num();
}


void UInventoryItemWidget::OnClicked()
{
	if (Owner != nullptr)
	{
		if (!GetIsEmpty())
		{
			Owner->SetActiveSlot(InventoryIndex);
		}
		else
		{
			Owner->HideActiveSlot();
		}
	}
}




void UInventoryItemWidget::UpdateDisplay()
{
	// Check for the components existing first
	if (DisplayText != nullptr && DisplayImage != nullptr)
	{
		if (!GetIsEmpty())
		{
			UInventoryObject* Object = PlayerState->Inventory[InventoryIndex].Object;
			
			if (Object->IconTexture.IsValid())
			{
				DisplayImage->SetVisibility(ESlateVisibility::Visible);
				DisplayImage->SetBrushFromTexture(Object->IconTexture.Get());
				
				DisplayText->SetVisibility(ESlateVisibility::Hidden);
			}
			else
			{
				DisplayImage->SetVisibility(ESlateVisibility::Hidden);

				DisplayText->SetVisibility(ESlateVisibility::Visible);
				DisplayText->SetText(Object->Name);
			}
		}
		else
		{
			DisplayImage->SetVisibility(ESlateVisibility::Hidden);
			DisplayText->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}



