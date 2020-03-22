// Copyright 2019 Spencer Melnick


#include "VDPlayerState.h"

#include "Net/UnrealNetwork.h"


void AVDPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AVDPlayerState, Inventory);
}

bool AVDPlayerState::TryPickupObject(FInventoryObject Object)
{
	switch(Object.Type)
	{
		case EInventoryObjectType::Unique:
			{
				bool bWasDuplicateFound = false;
				
				for (auto& i : Inventory)
				{
					if (i.ID == Object.ID)
					{
						bWasDuplicateFound = true;
						break;
					}
				}

				if (!bWasDuplicateFound)
				{
					Inventory.Add(Object);
				}
				else
				{
					// Object pickup fails if the player already has the unique object
					return false;
				}
			}
			break;

		case EInventoryObjectType::Stack:
			{
				bool bWasDuplicateFound = false;

				for (auto& i : Inventory)
				{
					if (i.ID == Object.ID)
					{
						bWasDuplicateFound = true;
						i.Count++;
						break;
					}
				}

				if (!bWasDuplicateFound)
				{
					Inventory.Add(Object);
				}
			}
			break;

		case EInventoryObjectType::Multiple:
			Inventory.Add(Object);
			break;
	}

	return true;
}


void AVDPlayerState::OnRep_Inventory() {
    // Do something here!
}
