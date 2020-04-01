// Copyright 2019 Spencer Melnick


#include "VDPlayerState.h"

#include "Net/UnrealNetwork.h"


void AVDPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AVDPlayerState, Inventory);
}

bool AVDPlayerState::TryPickupObject(UInventoryObject* Object)
{
	FInventorySlot* FoundSlot = Inventory.FindByPredicate([Object](const FInventorySlot& Slot)
	{
		return Slot.Object == Object;
	});
	
	switch(Object->DuplicationRule)
	{
		case EInventoryDuplicationRule::Unique:
			{

				if (FoundSlot == nullptr)
				{
					AddToInventory(Object);
				}
				else
				{
					// Object pickup fails if the player already has the unique object
					return false;
				}
			}
			break;

		case EInventoryDuplicationRule::Stack:
			{
				if (FoundSlot == nullptr)
				{
					AddToInventory(Object);
				}
				else
				{
					FoundSlot->Count++;
				}
			}
			break;

		case EInventoryDuplicationRule::Multiple:
			{
				AddToInventory(Object);
			}
			break;
	}

	return true;
}


void AVDPlayerState::OnRep_Inventory() {
    // Do something here!
}

void AVDPlayerState::AddToInventory(UInventoryObject* Object)
{
	FInventorySlot Slot;
	Slot.Object = Object;
	Inventory.Add(Slot);
	Object->HandlePickup();
}

