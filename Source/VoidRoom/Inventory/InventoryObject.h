// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class VOIDROOM_API InventoryObject
{
public:
	InventoryObject(FString Name, int16 ItemID);
	~InventoryObject();

	int16 GetItemID() const;

private:
	FString Name;
	int16 ItemID;
};
