// Copyright 2019 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Tool.generated.h"

UINTERFACE(MinimalAPI)
class UTool : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class VOIDROOM_API ITool
{
	GENERATED_BODY()

public:
	virtual void OnFire(class AVDCharacter* Character) = 0;
};
