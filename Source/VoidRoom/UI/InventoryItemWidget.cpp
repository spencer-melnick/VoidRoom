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
	if (DisplayText != nullptr)
	{
		if (!bIsEmpty)
		{
			DisplayText->SetText(FText::AsCultureInvariant(Object.Name));
		}
		else
		{
			DisplayText->SetText(FText::AsCultureInvariant(""));
		}
	}
}



