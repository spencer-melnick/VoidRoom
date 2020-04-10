// Copyright 2019 Spencer Melnick


#include "FlashlightTool.h"

#include "VoidRoom/VoidRoom.h"
#include "VoidRoom/Character/VDCharacter.h"

// Sets default values
UFlashlightTool::UFlashlightTool()
{
	
}


void UFlashlightTool::OnUse(AVDCharacter* Character)
{
	bIsOn = !bIsOn;

	UE_LOG(LogVD, Display, TEXT("%s is toggling flashlight"), *GetNameSafe(Character));
}


