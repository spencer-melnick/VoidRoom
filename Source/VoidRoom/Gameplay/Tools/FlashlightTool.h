// Copyright 2019 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "VoidRoom/Gameplay/Tools/Tool.h"

#include "FlashlightTool.generated.h"

UCLASS()
class VOIDROOM_API UFlashlightTool : public UTool
{
	GENERATED_BODY()
	
public:	
	UFlashlightTool();

	virtual void OnFire(class AVDCharacter* Character) override;

private:
	bool bIsOn = false;
	
};
