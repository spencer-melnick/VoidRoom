// Copyright 2019 Spencer Melnick

#pragma once

#include "CoreMinimal.h"

#include "VoidRoom/Gameplay/Inventory/InventoryBehavior.h"

#include "TestBehavior.generated.h"

/**
 * 
 */
UCLASS()
class VOIDROOM_API UTestBehavior: public UInventoryBehavior
{
	GENERATED_BODY()
	
public:
	virtual void OnPickup(TSoftObjectPtr<UInventoryObject> Object) override;
	virtual void OnDrop(TSoftObjectPtr<UInventoryObject> Object) override;
};
