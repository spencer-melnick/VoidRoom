// Copyright 2019 Spencer Melnick


#include "InventoryItemWidget.h"

void UInventoryItemWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	
	UpdateDisplay();
}

void UInventoryItemWidget::SetObject(FInventoryObject NewObject)
{
	bIsEmpty = false;
	Object = NewObject;

	UpdateDisplay();
}

void UInventoryItemWidget::SetEmpty()
{
	bIsEmpty = true;
	Object = FInventoryObject();

	UpdateDisplay();
}


void UInventoryItemWidget::UpdateDisplay()
{
	if (DisplayText != nullptr && DisplayImage != nullptr)
	{
		if (!bIsEmpty)
		{
			if (Object.IconTexture.IsValid())
			{
				DisplayImage->SetVisibility(ESlateVisibility::Visible);
				DisplayImage->SetBrushFromTexture(Object.IconTexture.Get());
			}
			else
			{
				DisplayImage->SetVisibility(ESlateVisibility::Hidden);
			}

			DisplayText->SetText(FText::AsCultureInvariant(Object.Name));
		}
		else
		{
			DisplayImage->SetVisibility(ESlateVisibility::Hidden);
			DisplayText->SetText(FText::AsCultureInvariant(""));
		}
	}
}



