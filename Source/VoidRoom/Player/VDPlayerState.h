// Copyright 2019 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "../Gameplay/Inventory/InventoryObject.h"
#include "VDPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class VOIDROOM_API AVDPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, Replicated)
	TArray<FInventoryObject> Inventory;

protected:
	UFUNCTION()
	virtual void OnRep_Inventory();
};
