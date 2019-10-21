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

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	// VD interfaces

	USceneComponent* GetViewAttachment() const;
	UCameraComponent* GetFirstPersonCamera() const;
	float GetCurrentEyeHeightFromCenter() const;
	float GetCurrentEyeHeightFromGround() const;
	FVector GetViewLocation() const;

	void StartCrouch();
	void StartUncrouch();
	void ToggleCrouch();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = View)
	float CrouchSpeed = 300.f;

protected:
	virtual void BeginPlay() override;

	void CheckCrouch();
	void AdjustEyeHeight(float DeltaTime);
	void UpdateViewRotation();

	USceneComponent* ViewAttachment;
	UCameraComponent* FirstPersonCamera;
	float EyeHeightFromGround;
	bool bAttemptingCrouch = false;

};
