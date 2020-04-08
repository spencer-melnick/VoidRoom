// Copyright 2019 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/Texture2D.h"

#include "VoidRoom/Gameplay/Inventory/InventoryBehavior.h"

#include "InventoryObject.generated.h"

UENUM(BlueprintType)
enum class EInventoryDuplicationRule : uint8
{
	Unique,			// You can only pick up one of a unique object
	Multiple,		// You can have duplicates of a "multiple" object
	Stack			// You can only pick up one of a stack object, but each new object increases the count
};

UCLASS()
class VOIDROOM_API UInventoryObject: public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FText Name;

	UPROPERTY(EditAnywhere)
	FText Description;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UTexture2D> IconTexture;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UInventoryBehavior> InventoryBehavior;

	UPROPERTY(EditAnywhere)
	EInventoryDuplicationRule DuplicationRule;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UTool> ToolComponent;

	void HandlePickup();
};
