// Copyright 2019 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "LevelSequenceActor.h"

/**
 * 
 */

class VOIDROOM_API SequenceUtilities
{
public:
	// No constructor or destructor - static functions only
	SequenceUtilities() = delete;
	~SequenceUtilities() = delete;

	static void RebindActorInSequence(ALevelSequenceActor* SequenceActor, FString FromActorName, AActor* ToActor);
};
