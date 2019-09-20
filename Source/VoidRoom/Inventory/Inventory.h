// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Inventory/InventoryObject.h"
#include "Inventory.generated.h"

/**
 * 
 */
UCLASS()
class VOIDROOM_API AInventory : public APlayerState
{
	GENERATED_BODY()
public:
	AInventory();

private: 
	TArray<InventoryObject> Objects;

public:
	void AddItem(InventoryObject Item);
	bool HasItem(InventoryObject Item);
	InventoryObject* GetItemByID(int16 ItemID);
};
