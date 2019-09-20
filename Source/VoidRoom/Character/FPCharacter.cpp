// Fill out your copyright notice in the Description page of Project Settings.


#include "FPCharacter.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
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

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
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

// Called to bind functionality to input
void AFPCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AFPCharacter::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	UE_LOG(LogTemp, Display, TEXT("Movement mode changed"));
}

