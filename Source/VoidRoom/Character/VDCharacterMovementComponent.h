// Copyright 2019 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "VDCharacterMovementComponent.generated.h"


// Forward declare character
class AVDCharacter;


/**
 * 
 */
UCLASS()
class VOIDROOM_API UVDCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UVDCharacterMovementComponent();

	// Engine overrides
	virtual void InitializeComponent() override;
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
	virtual void Crouch(bool bClientSimulation) override;
	virtual void UnCrouch(bool bClientSimulation) override;
	virtual float GetMaxSpeed() const override;
	
	// Public interface
	void CheckInitialOverlap(float DeltaSeconds);
	void UpdateCrouch(bool bClientSimulation, float DeltaSeconds);
	float GetCurrentHalfHeight() const;
	float GetCurrentHeight() const;

	// Public properties
	TEnumAsByte<EEasingFunc::Type> CrouchEaseFunction = EEasingFunc::SinusoidalInOut;
	float CrouchEaseTime = 0.25f;

private:
	// Cached pointer casts
	AVDCharacter* VoidCharacterOwner;

	// Private properties
	float CrouchEaseAlpha = 0.f;
};
