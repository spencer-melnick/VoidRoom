// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraAnim.h"
#include "Camera/CameraAnimInst.h"

#include "FPCharacter.generated.h"

UCLASS()
class VOIDROOM_API AFPCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Called when character is unpossessed by a controller
	virtual void UnPossessed() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float CrouchSpeed = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	UCameraAnim* ViewBobAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float ViewBobFadeTime = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float ViewBobScale = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float ViewBobSpeedScale = 2.0f;

private:
	UCameraComponent* CameraComponent;
	float CameraHeight;
	bool bWasWalking = false;
	UCameraAnimInst* ViewBobInst = nullptr;

	void AdjustCameraPosition(float DeltaTime);
	void AttemptViewBob();
};
