// Copyright 2019 Spencer Melnick


#include "InventoryItemWidget.h"

#include "VoidRoom/VoidRoom.h"
#include "InventoryGridWidget.h"

void UInventoryItemWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	
	UpdateDisplay();
}

void UInventoryItemWidget::SetInventorySlot(FInventorySlot NewSlot)
{
	bIsEmpty = false;
	InventorySlot = NewSlot;

	UpdateDisplay();
}

void UInventoryItemWidget::SetEmpty()
{
	bIsEmpty = true;
	InventorySlot = FInventorySlot();

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

void UInventoryItemWidget::OnClicked()
{
	if (!bIsEmpty && Owner != nullptr)
	{
		Owner->SetActiveSlot(InventorySlot);
	}
}




void UInventoryItemWidget::UpdateDisplay()
{	
	if (DisplayText != nullptr && DisplayImage != nullptr)
	{
		if (!bIsEmpty && InventorySlot.Object != nullptr)
		{
			UInventoryObject* Object = InventorySlot.Object;
			
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



