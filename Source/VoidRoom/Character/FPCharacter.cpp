// Fill out your copyright notice in the Description page of Project Settings.


#include "FPCharacter.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"

#include "Math/UnrealMathUtility.h"

// Sets default values
AFPCharacter::AFPCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Spawn camera and set height
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	CameraComponent->SetupAttachment(GetCapsuleComponent());
	CameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, BaseEyeHeight));

	// Enable crouching
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;
}

// Called when the game starts or when spawned
void AFPCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	CameraHeight = BaseEyeHeight;
}

// Called every frame
void AFPCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AdjustCameraPosition(DeltaTime);
	AttemptViewBob();
}

// Called to bind functionality to input
void AFPCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AFPCharacter::UnPossessed()
{
	if (ViewBobInst != nullptr)
	{
		// If we have a view bob animation playing, remove it 
		ViewBobInst->Stop();
		ViewBobInst = nullptr;
	}
}

void AFPCharacter::AdjustCameraPosition(float DeltaTime)
{
	// Make the camera face wherever the player controller (and by extension the characer) is looking
	CameraComponent->SetWorldRotation(GetViewRotation());

	float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	FVector CapsuleBase = FVector::DownVector * CapsuleHalfHeight;

	// Adjust eye height based on current state
	if (bIsCrouched)
	{
		CameraHeight -= CrouchSpeed * DeltaTime;
		CameraHeight = FMath::Max(CameraHeight, CapsuleHalfHeight + BaseEyeHeight);
	}
	else
	{
		CameraHeight += CrouchSpeed * DeltaTime;
		CameraHeight = FMath::Min(CameraHeight, CapsuleHalfHeight + BaseEyeHeight);	
	}
	

	// Set camera height to eye height
	CameraComponent->SetRelativeLocation(CapsuleBase + FVector::UpVector * CameraHeight);
}

void AFPCharacter::AttemptViewBob()
{
	if (IsPlayerControlled())
	{
		// Calculate how fast we are moving forward
		UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(GetMovementComponent());
		FVector LastVelocity = MovementComponent->GetLastUpdateVelocity();
		float ForwardSpeed = FMath::Abs(FVector::DotProduct(GetActorRotation().RotateVector(FVector::ForwardVector), LastVelocity));

		bool bCurrentlyWalking = false;

		// If we are walking (not falling, swimming, flying, etc.) and speed is greater than zero
		// we are walking during this frame
		if (LastVelocity.Size() > 0 && MovementComponent->MovementMode == EMovementMode::MOVE_Walking)
		{
			bCurrentlyWalking = true;
		}

		// If our walking state changed, update the camera animation
		if (bCurrentlyWalking != bWasWalking)
		{
			bWasWalking = bCurrentlyWalking;

			APlayerController* PlayerController = Cast<APlayerController>(GetController());

			if (bCurrentlyWalking)
			{
				// Apply a new camera animation if we're walking this frame
				ViewBobInst = PlayerController->PlayerCameraManager->PlayCameraAnim(
					ViewBobAnimation, ViewBobSpeedScale, ViewBobScale, ViewBobFadeTime, ViewBobFadeTime, true);
			}
			else
			{
				// Remove old camera animation if we're not walking this frame
				ViewBobInst->Stop();
				ViewBobInst = nullptr;
			}
		}

		// If there is an animation playing currently, adjust its speed based on how much of our
		// max speed is in the forward direction
		if (ViewBobInst != nullptr && bCurrentlyWalking)
		{
			float ForwardSpeedFactor = ForwardSpeed / MovementComponent->MaxWalkSpeed;
			ViewBobInst->PlayRate = ViewBobSpeedScale * ForwardSpeedFactor;
		}

	}
}
