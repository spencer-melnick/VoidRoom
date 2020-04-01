// Copyright 2019 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "InventoryBehavior.generated.h"

UCLASS()
class VOIDROOM_API UInventoryBehavior: public UObject
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void OnPickup(TSoftObjectPtr<class UInventoryObject> Object);
	virtual void OnDrop(TSoftObjectPtr<class UInventoryObject> Object);
};
