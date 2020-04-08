// Copyright 2019 Spencer Melnick


#include "Tool.h"

void UTool::OnFire(AVDCharacter* Character)
{
	
}


TSubclassOf<UAnimInstance> UTool::GetCharacterViewAnimationBlueprint() const
{
	return CharacterViewAnimationBlueprint;
}

FName UTool::GetAttachmentSocket() const
{
	return AttachmentSocket;
}


