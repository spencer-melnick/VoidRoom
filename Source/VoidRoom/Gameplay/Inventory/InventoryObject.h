// Copyright 2019 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "InventoryObject.generated.h"

USTRUCT()
struct VOIDROOM_API FInventoryObject
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FString Name;
	
};
