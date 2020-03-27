// Copyright 2019 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "InteractiveActor.h"

#include "Engine/DataTable.h"
#include "../Inventory/InventoryObject.h"

#include "ItemPickup.generated.h"

/**
 * 
 */
UCLASS()
class VOIDROOM_API AItemPickup : public AInteractiveActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UInventoryObject> Object;
	
protected:
	virtual void OnInteract(ACharacter* Character);
};
