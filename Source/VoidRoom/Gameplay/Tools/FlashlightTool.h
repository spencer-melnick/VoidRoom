// Copyright 2019 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "VoidRoom/Gameplay/Tools/Tool.h"

#include "FlashlightTool.generated.h"

UCLASS()
class VOIDROOM_API AFlashlightTool : public AActor, public ITool
{
	GENERATED_BODY()
	
public:	
	AFlashlightTool();

	virtual void Tick(float DeltaTime) override;

	virtual void OnFire(class AVDCharacter* Character) override;

protected:
	virtual void BeginPlay() override;

private:
	bool bIsOn = false;
	
};
