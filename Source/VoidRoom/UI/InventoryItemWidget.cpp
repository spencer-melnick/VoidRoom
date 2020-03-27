// Copyright 2019 Spencer Melnick


#include "InventoryItemWidget.h"

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


void UInventoryItemWidget::UpdateDisplay()
{
	if (DisplayText != nullptr && DisplayImage != nullptr)
	{
		if (!bIsEmpty && InventorySlot.Object.IsValid())
		{
			UInventoryObject* ObjectInstance = InventorySlot.Object.Get();
			
			if (ObjectInstance->IconTexture.IsValid())
			{
				DisplayImage->SetVisibility(ESlateVisibility::Visible);
				DisplayImage->SetBrushFromTexture(ObjectInstance->IconTexture.Get());
			}
			else
			{
				DisplayImage->SetVisibility(ESlateVisibility::Hidden);
			}

			DisplayText->SetText(FText::AsCultureInvariant(ObjectInstance->Name));
		}
		else
		{
			DisplayImage->SetVisibility(ESlateVisibility::Hidden);
			DisplayText->SetText(FText::AsCultureInvariant(""));
		}
	}
}



