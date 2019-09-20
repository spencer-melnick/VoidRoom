// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryObject.h"

InventoryObject::InventoryObject(FString Name, int16 ItemID)
{
}

InventoryObject::~InventoryObject()
{
}

int16 InventoryObject::GetItemID() const{
    return ItemID;
}