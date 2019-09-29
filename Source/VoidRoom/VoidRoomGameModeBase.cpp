// Fill out your copyright notice in the Description page of Project Settings.


#include "VoidRoomGameModeBase.h"


void AVoidRoomGameModeBase::StartPlay() {
    Super::StartPlay();

}

bool AVoidRoomGameModeBase::RegisterItem(int16 ID, FString Name){
  if (ItemMap.Contains(ID)){
    return false;
  }
  ItemMap.Add(ID, Name);
  return true;
}

FString AVoidRoomGameModeBase::GetItemByID(int16 ID){
  return *ItemMap.Find(ID);
}