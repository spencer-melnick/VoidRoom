// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"

#include "VDCharacterMovementComponent.h"

#include "VDCharacter.generated.h"

UCLASS()
class VOIDROOM_API AVDCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AVDCharacter(const FObjectInitializer& ObjectInitializer);

	// Public engine overrides
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Bound events
	UFUNCTION()
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	// VD interfaces
	USceneComponent* GetViewAttachment() const;
	UCameraComponent* GetFirstPersonCamera() const;
	UVDCharacterMovementComponent* GetCharacterMovementComponent() const;
	float GetCurrentEyeHeightFromCenter() const;
	float GetCurrentEyeHeightFromGround() const;
	FVector GetViewLocation() const;
	AActor* GetFocusedActor() const;

	void StartCrouch();
	void StartUncrouch();
	void ToggleCrouch();

	void Interact();

	// Editor properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = View)
	float CrouchSpeed = 300.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = View)
	float HeadToEyeHeight = 20.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Control)
	float MaxFocusDistance = 500.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Control)
	float LedgeCastDistance = 50.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Control)
	float LedgeCastRadius = 25.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Control)
	float MinLedgeHeight = 25.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Control)
	float MaxLedgeHeight = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Control)
	float MaxLedgeAngle = 30.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Control)
	float ClimbForwardDistance = 20.f;

protected:
	// Protected engine overrides
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

	// Helper functions
	void AdjustEyeHeight();
	void UpdateViewRotation();
	void CheckFocus();
	void UpdateTriggerCapsule();
	bool CheckForClimbableLedge(FVector& NewLocation);

private:
	// Attached components
	USceneComponent* ViewAttachment;
	UCameraComponent* FirstPersonCamera;
	UCapsuleComponent* TriggerCapsule;

	// Cached component casts
	UVDCharacterMovementComponent* CharacterMovementComponent;

	// Normal operating variables
	// UPROPERTY(Replicated)
	AActor* FocusedActor = nullptr;
};
