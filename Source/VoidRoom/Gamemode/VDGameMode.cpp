// Fill out your copyright notice in the Description page of Project Settings.


#include "VDGameMode.h"

#include "../Character/VDCharacter.h"
#include "../Controller/VDPlayerController.h"
#include "../Player/VDPlayerState.h"


AVDGameMode::AVDGameMode()
    : Super()
{
    DefaultPawnClass = AVDGameMode::StaticClass();
    PlayerControllerClass = AVDPlayerController::StaticClass();
    PlayerStateClass = AVDPlayerState::StaticClass();
}
