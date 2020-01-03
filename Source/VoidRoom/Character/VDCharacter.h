// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "VDCharacter.generated.h"

UCLASS()
class VOIDROOM_API AVDCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AVDCharacter();

	// Engine events
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	// VD interfaces
	USceneComponent* GetViewAttachment() const;
	UCameraComponent* GetFirstPersonCamera() const;
	float GetCurrentEyeHeightFromCenter() const;
	float GetCurrentEyeHeightFromGround() const;
	FVector GetViewLocation() const;
	AActor* GetFocusedActor() const;

	void StartCrouch();
	void StartUncrouch();
	void ToggleCrouch();

	void Interact();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = View)
	float CrouchSpeed = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Control)
	float MaxFocusDistance = 500.f;

protected:
	virtual void BeginPlay() override;

	// Helper functions
	void CheckCrouch();
	void AdjustEyeHeight(float DeltaTime);
	void UpdateViewRotation();
	void CheckFocus();
	void UpdateTriggerCapsule();

	USceneComponent* ViewAttachment;
	UCameraComponent* FirstPersonCamera;
	UCapsuleComponent* TriggerCapsule;
	float EyeHeightFromGround;
	bool bAttemptingCrouch = false;


	// UPROPERTY(Replicated)
	AActor* FocusedActor = nullptr;
};
