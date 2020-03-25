// Copyright 2019 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RoleDisplay.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class VOIDROOM_API URoleDisplay : public UUserWidget
{
	GENERATED_BODY()

public:
	URoleDisplay(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
