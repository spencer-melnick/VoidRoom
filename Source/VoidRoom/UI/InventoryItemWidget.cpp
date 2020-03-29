// Copyright 2019 Spencer Melnick


#include "InventoryItemWidget.h"

#include "VoidRoom/VoidRoom.h"
#include "InventoryGridWidget.h"

void UInventoryItemWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	
	UpdateDisplay();
	
	if (MainButton != nullptr)
	{
		MainButton->OnClicked.Clear();
		MainButton->OnClicked.AddDynamic(this, &UInventoryItemWidget::OnClicked);
	}
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
}

void UInventoryItemWidget::OnClicked()
{
	UE_LOG(LogVD, Display, TEXT("Button %s pressed!"), *GetNameSafe(this));
	
	if (!bIsEmpty && Owner != nullptr)
	{
		Owner->SetActiveSlot(InventorySlot);
	}
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
				
				DisplayText->SetVisibility(ESlateVisibility::Hidden);
			}
			else
			{
				DisplayImage->SetVisibility(ESlateVisibility::Hidden);

				DisplayText->SetVisibility(ESlateVisibility::Visible);
				DisplayText->SetText(ObjectInstance->Name);
			}
		}
		else
		{
			DisplayImage->SetVisibility(ESlateVisibility::Hidden);
			DisplayText->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}



