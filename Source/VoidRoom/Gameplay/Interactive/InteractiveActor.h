// Copyright 2019 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractiveActor.generated.h"

// Forward declare character
class ACharacter;

UCLASS()
class VOIDROOM_API AInteractiveActor : public AActor
{
	GENERATED_BODY()
	
public:
	AInteractiveActor(const FObjectInitializer& ObjectInitializer);

	// VD public interfaces
	virtual void LocalFocused(ACharacter* Character);
	virtual void LocalUnfocused(ACharacter* Character);

	// This is not networked, but it should only be called from the server!
	void ServerInteract(ACharacter* Character);

	// Networked functions
	UFUNCTION(Reliable, NetMulticast)
	void MulticastInteract(ACharacter* Character);

protected:
	// VD interfaces
	virtual void OnLocalFocused(ACharacter* Character);
	virtual void OnLocalUnfocused(ACharacter* Character);
	virtual void OnInteract(ACharacter* Character);
	virtual bool GetShouldRunOnAllClients() const;

	// Helper functions
	void ShowHighlight();
	void HideHighlight();
};
