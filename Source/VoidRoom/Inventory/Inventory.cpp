// Fill out your copyright notice in the Description page of Project Settings.

#include "Inventory.h"


AInventory::AInventory(){
    Objects = TArray<InventoryObject>();
}

void AInventory::AddItem(InventoryObject Item) {
    Objects.Add(Item);
}

InventoryObject* AInventory::GetItemByID(int16 ItemID) {
    for (auto& item : Objects){
        if (item.GetItemID() == ItemID) {
            return &item;
        }
    }
    return nullptr;
}

bool AInventory::HasItem(InventoryObject Item) {
    return false;
}