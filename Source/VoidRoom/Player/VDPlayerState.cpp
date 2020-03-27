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
	
	switch(Object->DuplicationRule)
	{
		case EInventoryDuplicationRule::Unique:
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
					TryHandlePickup(Object);
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
					TryHandlePickup(Object);
				}
			}
			break;

		case EInventoryDuplicationRule::Multiple:
			{
				FInventorySlot Slot;
				Slot.Object = Object;
				Inventory.Add(Slot);
				TryHandlePickup(Object);
			}
			break;
	}

	return true;
}


void AVDPlayerState::OnRep_Inventory() {
    // Do something here!
}

void AVDPlayerState::TryHandlePickup(TSoftObjectPtr<UInventoryObject> Object)
{
	if (Object.IsValid())
	{
		UInventoryObject* ObjectInstance = Object.Get();

		if (ObjectInstance->InventoryBehavior != nullptr)
		{
			UInventoryBehavior* BehaviorInstance = ObjectInstance->InventoryBehavior.GetDefaultObject();

			if (BehaviorInstance != nullptr)
			{
				BehaviorInstance->OnPickup(Object);
			}
		}
	}
}

