// Copyright 2019 Spencer Melnick


#include "FlashlightTool.h"

#include "VoidRoom/VoidRoom.h"
#include "VoidRoom/Character/VDCharacter.h"

// Sets default values
AFlashlightTool::AFlashlightTool()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AFlashlightTool::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFlashlightTool::BeginPlay()
{
	Super::BeginPlay();

}

void AFlashlightTool::OnFire(AVDCharacter* Character)
{
	bIsOn = !bIsOn;

	UE_LOG(LogVD, Display, TEXT("%s is toggling flashlight"), *GetNameSafe(Character));
}


