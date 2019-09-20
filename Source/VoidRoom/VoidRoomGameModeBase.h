// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "VoidRoomGameModeBase.generated.h"


/**
 * 
 */
UCLASS()
class VOIDROOM_API AVoidRoomGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	virtual void StartPlay() override;

private:
	
};
