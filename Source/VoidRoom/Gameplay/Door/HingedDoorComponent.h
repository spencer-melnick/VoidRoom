// Copyright 2019 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/InteractableComponent.h"

#include "HingedDoorComponent.generated.h"

UCLASS( ClassGroup=(VoidRoom), meta=(BlueprintSpawnableComponent) )
class VOIDROOM_API UHingedDoorComponent : public UInteractableComponent
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	virtual void OnInteract(AVDCharacter* Character) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Door)
	bool bIsOpen = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Door)
	float OpenAngle = 90.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Door)
	float ClosedAngle = 0.0f;

protected:
	void UpdateHinge();
	void SetAngle(float Angle);
};
