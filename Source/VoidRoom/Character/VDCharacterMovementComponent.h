// Copyright 2019 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/InterpCurve.h"

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
	virtual void StartNewPhysics(float deltaTime, int32 Iterations) override;

	// Engine overrides for networked movement
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	virtual void OnMovementUpdated(float DeltaTime, const FVector& OldLocation, const FVector& OldVelocity) override;
	
	// Public interface
	void CheckInitialOverlap(float DeltaSeconds);
	void UpdateCrouch(bool bClientSimulation, float DeltaSeconds);
	float GetCurrentHalfHeight() const;
	float GetCurrentHeight() const;
	bool IsClimbing() const;

	void ClimbLedge(FVector InitialPosition, FVector WallPosition, FVector LedgePosition);

	// Public properties
	// Crouching
	TEnumAsByte<EEasingFunc::Type> CrouchEaseFunction = EEasingFunc::SinusoidalInOut;
	float CrouchEaseTime = 0.25f;

	// Climbing
	TEnumAsByte<EEasingFunc::Type> ClimbEaseFunction = EEasingFunc::SinusoidalInOut;
	float ClimbEaseTime = 0.5f;


	// Ledge climb
	bool bWantsToClimb = false;
	FVector ClimbInitialPosition;
	FVector ClimbWallPosition;
	FVector ClimbLedgePosition;

	// Movement modes
	static const uint8 MOVE_Climb = 0;

protected:
	// Custom movement mode
	void PhysClimbing(float deltaTime, int32 Iterations);

	// Networked move interfaces
	UFUNCTION(Unreliable, Server, WithValidation)
	void ServerClimbLedge(const FVector InitialPosition, const FVector WallPosition, const FVector LedgePosition);

private:
	// Cached pointer casts
	AVDCharacter* VoidCharacterOwner;

	// Private properties
	// Crouch
	float CrouchEaseAlpha = 0.f;

	// Ledge climb
	FInterpCurveVector ClimbCurve;
	float ClimbEaseAlpha = 0.f;
};


// Custom saved moves
class VOIDROOM_API FSavedMove_VDCharacter: public FSavedMove_Character
{
public:
	typedef FSavedMove_Character Super;

	// virtual bool NeedsRotationSent() const;

	virtual void Clear() override;
	virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, const FVector& NewAccel,
		FNetworkPredictionData_Client_Character& ClientData) override;
	virtual uint8 GetCompressedFlags() const override;
	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const override;
	// virtual bool IsImportantMove(const FSavedMovePtr& LastAckedMove) const override;
	// virtual bool IsCriticalMove(const FSavedMovePtr& LastAckedMove) const;
	// virtual void PostUpdate(ACharacter* C, EPostUpdateMode PostUpdateMode) override;
	virtual void PrepMoveFor(ACharacter* C) override;

	// Ledge climb
	bool bSavedWantsToClimb;
	FVector SavedClimbInitialPosition;
	FVector SavedClimbWallPosition;
	FVector SavedClimbLedgePosition;
};

class VOIDROOM_API FNetworkPredictionData_Client_VDCharacter: public FNetworkPredictionData_Client_Character
{
public:
	typedef FNetworkPredictionData_Client_Character Super;

	FNetworkPredictionData_Client_VDCharacter(const UVDCharacterMovementComponent& MovementComponent);

	/** Allocate a new saved move. Subclasses should override this if they want to use a custom move class. */
	virtual FSavedMovePtr AllocateNewMove() override;
};
