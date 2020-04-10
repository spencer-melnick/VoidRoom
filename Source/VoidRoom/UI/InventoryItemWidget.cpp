// Copyright 2019 Spencer Melnick


#include "InventoryItemWidget.h"

#include "VoidRoom/VoidRoom.h"
#include "VoidRoom/Player/VDPlayerState.h"
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

FInventorySlot* UInventoryItemWidget::GetAssociatedSlot() const
{
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

	if (InventoryIndex >= Inventory.Num())
	{
		return nullptr;
	}

	return &Inventory[InventoryIndex];
}



void UInventoryItemWidget::OnClicked()
{
	if (Owner != nullptr)
	{
		if (GetAssociatedSlot() != nullptr)
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
		FInventorySlot* InventorySlot = GetAssociatedSlot();
		
		if (InventorySlot != nullptr)
		{
			UInventoryObject* Object = InventorySlot->Object;
			
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

