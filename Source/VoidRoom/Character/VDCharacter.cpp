// Fill out your copyright notice in the Description page of Project Settings.


#include "VDCharacter.h"

#include "Math/UnrealMathUtility.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "../VoidRoom.h"
#include "../Gameplay/InteractableComponent.h"

AVDCharacter::AVDCharacter()
{
 	// Set this character to call Tick() every frame
	PrimaryActorTick.bCanEverTick = true;

	// Set starting eye height
	EyeHeightFromGround = GetDefaultHalfHeight() + BaseEyeHeight;

	// Spawn view attachment
	ViewAttachment = CreateDefaultSubobject<USceneComponent>(TEXT("ViewAttachment"));
	ViewAttachment->SetupAttachment(GetRootComponent());

	// Spawn first person camera
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(ViewAttachment);
	FirstPersonCamera->FieldOfView = 90.f;
	UpdateViewRotation();

	// Set movement component parameters
	UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(GetMovementComponent());
	if (MovementComponent == nullptr)
	{
		UE_LOG(LogVD, Warning, TEXT("VDCharacter %s does not have a character movement component"), *GetNameSafe(this));
	}
	else
	{
		MovementComponent->NavAgentProps.bCanCrouch = true;
		MovementComponent->bCanWalkOffLedges = true;
		MovementComponent->bCanWalkOffLedgesWhenCrouching = true;
	}

	// Setup overlap events
	TriggerCapsule = CreateDefaultSubobject<UCapsuleComponent>("TriggerCapsule");
	TriggerCapsule->AttachTo(GetCapsuleComponent());
	TriggerCapsule->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerCapsule->SetCollisionProfileName("Trigger");
	TriggerCapsule->SetGenerateOverlapEvents(true);
	TriggerCapsule->OnComponentBeginOverlap.AddDynamic(this, &AVDCharacter::OnBeginOverlap);
	UpdateTriggerCapsule();
}


// Overrides of public interface

void AVDCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckCrouch();
	AdjustEyeHeight(DeltaTime);
	UpdateViewRotation();
	CheckFocus();
	UpdateTriggerCapsule();
}

void AVDCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AVDCharacter::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("PushCharacter"))
	{
		// Minor hack to resolve penetration against moving objects
		FHitResult HitResult;
		GetMovementComponent()->SafeMoveUpdatedComponent(OtherActor->GetVelocity() * 0.01f, GetActorRotation(), true, HitResult);
	}
}



// VD interface

USceneComponent* AVDCharacter::GetViewAttachment() const
{
	return ViewAttachment;
}

UCameraComponent* AVDCharacter::GetFirstPersonCamera() const
{
	return FirstPersonCamera;
}

float AVDCharacter::GetCurrentEyeHeightFromCenter() const
{
	return GetCurrentEyeHeightFromGround() - GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
}

float AVDCharacter::GetCurrentEyeHeightFromGround() const
{
	return EyeHeightFromGround;
}

FVector AVDCharacter::GetViewLocation() const
{
	return ViewAttachment->GetComponentLocation();
}

AActor* AVDCharacter::GetFocusedActor() const
{
	return FocusedActor;
}



void AVDCharacter::StartCrouch()
{
	bAttemptingCrouch = true;
}

void AVDCharacter::StartUncrouch()
{
	bAttemptingCrouch = false;
}

void AVDCharacter::ToggleCrouch()
{
	bAttemptingCrouch = !bAttemptingCrouch;
}

void AVDCharacter::Interact()
{
	if (FocusedActor != nullptr)
	{
		TInlineComponentArray<UInteractableComponent*> InteractableComponents(FocusedActor);
		FocusedActor->GetComponents<UInteractableComponent>(InteractableComponents, true);

		for (auto& i : InteractableComponents)
		{
			if (i != nullptr)
			{
				i->OnInteract(this);
			}
		}
	}
}


// Overrides of protected interface

void AVDCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}


// VD protected interface

void AVDCharacter::CheckCrouch()
{
	UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(GetMovementComponent());
	if (MovementComponent == nullptr) {
		UE_LOG(LogVD, Warning, TEXT("VDCharacter %s does not have a character movement component"), *GetNameSafe(this));
		return;
	}

	float CrouchedEyeHeightFromGround = CrouchedEyeHeight + MovementComponent->CrouchedHalfHeight;

	// If our animation has completed (camera height = desired crouched eye height)
	// notify the movement component to change capsule height
	if (GetCurrentEyeHeightFromGround() == CrouchedEyeHeightFromGround && !bIsCrouched && bAttemptingCrouch)
	{
		Crouch();
	}
	// If we're currently crouched, and stopped attempting for any reason, notify
	// the movement component to reset capsule height
	else if (bIsCrouched && !bAttemptingCrouch)
	{
		UnCrouch();
	}
}

void AVDCharacter::AdjustEyeHeight(float DeltaTime)
{
	UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(GetMovementComponent());

	if (MovementComponent == nullptr)
	{
		UE_LOG(LogVD, Warning, TEXT("VDCharacter %s does not have a character movement component"), *GetNameSafe(this));
		return;
	}

	float StandingHalfHeight = GetDefaultHalfHeight();
	float CrouchedHalfHeight = MovementComponent->CrouchedHalfHeight;

	if (bAttemptingCrouch || bIsCrouched)
	{
		// Figure out the limits of the camera height (from the ground) using the final crouched height
		float CrouchedEyeHeightFromGround = MovementComponent->CrouchedHalfHeight + CrouchedEyeHeight;
		EyeHeightFromGround -= CrouchSpeed * DeltaTime;
		EyeHeightFromGround = FMath::Max<float>(EyeHeightFromGround, CrouchedEyeHeightFromGround);
	}
	else
	{
		// Figure out the limits of the camera height (from the ground) using the default height
		float StandingEyeHeightFromGround = StandingHalfHeight + BaseEyeHeight;
		EyeHeightFromGround += CrouchSpeed * DeltaTime;
		EyeHeightFromGround = FMath::Min<float>(EyeHeightFromGround, StandingEyeHeightFromGround);
	}

	// Set camera height relative to ground using current capsule half height
	GetViewAttachment()->SetRelativeLocation(FVector::UpVector * GetCurrentEyeHeightFromCenter());
}

void AVDCharacter::UpdateViewRotation()
{
	FRotator ControlRotation = GetControlRotation();
	GetViewAttachment()->SetRelativeRotation(FRotator(ControlRotation.Pitch, 0.f, 0.f));
}

void AVDCharacter::CheckFocus()
{
	// Setup line trace start and end
	FVector TraceStart = ViewAttachment->GetComponentLocation();
	FVector TraceOffset = ViewAttachment->GetComponentRotation().RotateVector(FVector::ForwardVector);
	TraceOffset *= MaxFocusDistance;
	FVector TraceEnd = TraceStart + TraceOffset;

	// Perform trace against visibility channel
	AActor* HitActor = nullptr;
	FHitResult HitResult;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility))
	{
		HitActor = HitResult.GetActor();
	}

	// If we're looking at a new actor
	if (FocusedActor != HitActor)
	{
		// Notify all of the previous interactable components they have been unfocused
		if (FocusedActor != nullptr)
		{
			TInlineComponentArray<UInteractableComponent*> InteractableComponents(FocusedActor);
			FocusedActor->GetComponents<UInteractableComponent>(InteractableComponents, true);

			for (auto& i : InteractableComponents)
			{
				if (i != nullptr)
				{
					i->OnUnfocused();
				}
			}
		}

		// Notify all the new interactable components they have been focused
		FocusedActor = HitActor;
		if (FocusedActor != nullptr)
		{
			TInlineComponentArray<UInteractableComponent*> InteractableComponents(this);
			FocusedActor->GetComponents<UInteractableComponent>(InteractableComponents, true);

			for (auto& i : InteractableComponents)
			{
				if (i != nullptr)
				{
					i->OnFocused();
				}
			}
		}
	}
}

void AVDCharacter::UpdateTriggerCapsule()
{
	UCapsuleComponent* RegularCapsule = GetCapsuleComponent();

	if (TriggerCapsule != nullptr && RegularCapsule != nullptr)
	{
		TriggerCapsule->SetCapsuleSize(RegularCapsule->GetUnscaledCapsuleRadius(), RegularCapsule->GetUnscaledCapsuleHalfHeight());
	}
}
