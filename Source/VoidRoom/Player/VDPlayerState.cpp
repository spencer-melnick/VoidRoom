// Copyright 2019 Spencer Melnick


#include "VDPlayerState.h"

#include "Net/UnrealNetwork.h"


void AVDPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AVDPlayerState, Inventory);
}

bool AVDPlayerState::TryPickupObject(TSoftObjectPtr<UInventoryObject> Object)
{
	UInventoryObject* ObjectInstance = Object.Get();
	
	switch(Object->DuplicationType)
	{
		case EInventoryDuplicationType::Unique:
			{
				bool bWasDuplicateFound = false;
				
				for (auto& i : Inventory)
				{
					if (i.Object.GetUniqueID() == Object.GetUniqueID())
					{
						bWasDuplicateFound = true;
						break;
					}
				}

				if (!bWasDuplicateFound)
				{
					FInventorySlot Slot;
					Slot.Object = Object;
					Inventory.Add(Slot);
				}
				else
				{
					// Object pickup fails if the player already has the unique object
					return false;
				}
			}
			break;

		case EInventoryDuplicationType::Stack:
			{
				bool bWasDuplicateFound = false;

				for (auto& i : Inventory)
				{
					if (i.Object.GetUniqueID() == Object.GetUniqueID())
					{
						bWasDuplicateFound = true;
						i.Count++;
						break;
					}
				}

				if (!bWasDuplicateFound)
				{
					FInventorySlot Slot;
					Slot.Object = Object;
					Inventory.Add(Slot);
				}
			}
			break;

		case EInventoryDuplicationType::Multiple:
			{
				FInventorySlot Slot;
				Slot.Object = Object;
				Inventory.Add(Slot);
			}
			break;
	}

	return true;
}


void AVDPlayerState::OnRep_Inventory() {
    // Do something here!
}
