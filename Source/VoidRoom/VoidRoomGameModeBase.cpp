// Fill out your copyright notice in the Description page of Project Settings.


#include "VoidRoomGameModeBase.h"


void AVoidRoomGameModeBase::StartPlay() {
    Super::StartPlay();

    if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("This is an on screen message!"));
    }
}

