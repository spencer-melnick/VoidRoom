// Copyright 2019 Spencer Melnick


#include "InventoryItemWidget.h"

void UInventoryItemWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	
	UpdateDisplay();
}

void UInventoryItemWidget::SetObject(FInventorySlot NewObject)
{
	bIsEmpty = false;
	Object = NewObject;

	UpdateDisplay();
}

void UInventoryItemWidget::SetEmpty()
{
	bIsEmpty = true;
	Object = FInventorySlot();

	UpdateDisplay();
}


void UInventoryItemWidget::UpdateDisplay()
{
	if (DisplayText != nullptr && DisplayImage != nullptr)
	{
		if (!bIsEmpty && Object.Object.IsValid())
		{
			UInventoryObject* ObjectInstance = Object.Object.Get();
			
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



