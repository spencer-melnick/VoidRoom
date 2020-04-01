// Copyright 2019 Spencer Melnick


#include "InventoryObject.h"

void UInventoryObject::HandlePickup()
{
	if (InventoryBehavior != nullptr)
	{
		UInventoryBehavior* BehaviorInstance = InventoryBehavior.GetDefaultObject();

		BehaviorInstance->OnPickup(TSoftObjectPtr<UInventoryObject>(this));
	}
}

