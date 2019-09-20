// Fill out your copyright notice in the Description page of Project Settings.


#include "FPCharacter.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"

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
	
}

// Called every frame
void AFPCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Make the camera face wherever the player controller (and by extension the characer) is looking
	CameraComponent->SetWorldRotation(GetViewRotation());

	// Adjust eye height based on current state
	float EyeHeight = BaseEyeHeight;
	if (bIsCrouched)
	{
		EyeHeight = CrouchedEyeHeight;
	}

	// Set camera height to eye height
	CameraComponent->SetRelativeLocation(FVector::UpVector * EyeHeight);
}

// Called to bind functionality to input
void AFPCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AFPCharacter::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	UE_LOG(LogTemp, Warning, TEXT("Movement mode changed"));
}

