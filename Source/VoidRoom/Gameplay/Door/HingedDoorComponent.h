// Copyright 2019 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "Kismet/KismetMathLibrary.h"

#include "../InteractableComponent.h"

#include "HingedDoorComponent.generated.h"


UCLASS( ClassGroup=(VoidRoom), meta=(BlueprintSpawnableComponent) )
class VOIDROOM_API UHingedDoorComponent : public UInteractableComponent
{
	GENERATED_BODY()

public:
	UHingedDoorComponent();

	// Engine events
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void BeginPlay() override;

	// VoidRoom events
	virtual void OnInteract(AVDCharacter* Character) override;

	// Blueprint properties
	// Door properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Door)
	bool bIsOpen = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Door)
	float OpenAngle = 90.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Door)
	float ClosedAngle = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Door)
	FString OriginComponentName = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Door)
	FString DoorComponentName = "";

	// Transition properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Transition)
	float TransitionTime = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Transition)
	float ClosedThreshold = 0.01f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Transition)
	TEnumAsByte<EEasingFunc::Type> EasingFunction = EEasingFunc::SinusoidalOut;

protected:
	float GetDesiredAngle() const;

private:
	USceneComponent* OriginComponent = nullptr;
	UPrimitiveComponent* DoorComponent = nullptr;
	float TransitionAlpha = 0.f;
};
