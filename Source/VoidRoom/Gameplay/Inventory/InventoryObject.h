// Copyright 2019 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "InventoryObject.generated.h"

UCLASS()
class VOIDROOM_API UInventoryObject : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FString Name;
	
};
