// Copyright 2019 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "Engine/Texture2D.h"

#include "InventoryObject.generated.h"

UENUM(BlueprintType)
enum class EInventoryObjectType : uint8
{
	Unique,			// You can only pick up one of a unique object
	Multiple,		// You can have duplicates of a "multiple" object
	Stack			// You can only pick up one of a stack object, but each new object increases the count
};

USTRUCT()
struct VOIDROOM_API FInventoryObject: public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	int ID;

	UPROPERTY(EditAnywhere)
	FString Name;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UTexture2D> IconTexture;

	UPROPERTY(EditAnywhere)
	int Count = 1;

	UPROPERTY(EditAnywhere)
	EInventoryObjectType Type;
};
