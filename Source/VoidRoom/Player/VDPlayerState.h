// Copyright 2019 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "../Gameplay/Inventory/InventoryObject.h"
#include "VDPlayerState.generated.h"

/**
 * 
 */
USTRUCT()
struct FInventorySlot
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UInventoryObject> Object;

	UPROPERTY(EditAnywhere)
	int Count = 1;
};

UCLASS()
class VOIDROOM_API AVDPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, Replicated)
	TArray<FInventorySlot> Inventory;

	bool TryPickupObject(TSoftObjectPtr<UInventoryObject> Object);

protected:
	UFUNCTION()
	virtual void OnRep_Inventory();

	void TryHandlePickup(TSoftObjectPtr<UInventoryObject> Object);
};
