// Copyright 2019 Spencer Melnick


#include "KnifeTool.h"

void UKnifeTool::OnUse(AVDCharacter* Character)
{
	OnStartAttackDelegate.Broadcast();
}
