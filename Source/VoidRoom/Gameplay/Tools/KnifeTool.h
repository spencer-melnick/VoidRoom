// Copyright 2019 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "VoidRoom/Gameplay/Tools/Tool.h"
#include "KnifeTool.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStartAttackDelegate);

UCLASS()
class VOIDROOM_API UKnifeTool : public UTool
{
	GENERATED_BODY()

public:
	virtual void OnUse(AVDCharacter* Character) override;
	
	UPROPERTY(BlueprintAssignable, Category = Knife)
	FStartAttackDelegate OnStartAttackDelegate;
};
