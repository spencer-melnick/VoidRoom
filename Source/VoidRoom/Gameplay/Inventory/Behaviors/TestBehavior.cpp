// Copyright 2019 Spencer Melnick


#include "TestBehavior.h"

#include "VoidRoom/VoidRoom.h"
#include "VoidRoom/Gameplay/Inventory/InventoryObject.h"

void UTestBehavior::OnPickup(TSoftObjectPtr<UInventoryObject> Object)
{
	if (Object.IsValid())
	{
		UInventoryObject* ObjectInstance = Object.Get();

		UE_LOG(LogVD, Display, TEXT("Player picked up a %s"), *ObjectInstance->Name.ToString());
	}
}


void UTestBehavior::OnDrop(TSoftObjectPtr<UInventoryObject> Object)
{
	
}
