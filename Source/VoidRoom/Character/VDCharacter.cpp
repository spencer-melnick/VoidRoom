// Fill out your copyright notice in the Description page of Project Settings.


#include "VDCharacter.h"

#include "Math/UnrealMathUtility.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "DrawDebugHelpers.h"

#include "../VoidRoom.h"
#include "../Gameplay/InteractableComponent.h"

AVDCharacter::AVDCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UVDCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame
	PrimaryActorTick.bCanEverTick = true;

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
}


// Overrides of public interface

void AVDCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UCapsuleComponent* Capsule = GetCapsuleComponent();

	if (IsLocallyControlled())
	{
		// Only check focus on controlled characters
		CheckFocus();
	}
	else
	{
		// Draw a debug capsule for other characters
		DrawDebugCapsule(GetWorld(), Capsule->GetComponentLocation(), Capsule->GetScaledCapsuleHalfHeight(),
			Capsule->GetScaledCapsuleRadius(), FQuat::Identity, FColor::Green);
	}

	AdjustEyeHeight();
	UpdateViewRotation();
}

void AVDCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AVDCharacter::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}



// VD public interface

USceneComponent* AVDCharacter::GetViewAttachment() const
{
	return ViewAttachment;
}

UCameraComponent* AVDCharacter::GetFirstPersonCamera() const
{
	return FirstPersonCamera;
}

UVDCharacterMovementComponent* AVDCharacter::GetCharacterMovementComponent() const
{
	return CharacterMovementComponent;
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
	CharacterMovementComponent->bWantsToCrouch = true;
}

void AVDCharacter::StartUncrouch()
{
	CharacterMovementComponent->bWantsToCrouch = false;
}

void AVDCharacter::ToggleCrouch()
{
	CharacterMovementComponent->bWantsToCrouch = !CharacterMovementComponent->bWantsToCrouch;
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

void AVDCharacter::TryClimbLedge()
{
	FVector WallLocation;
	FVector LedgeLocation;

	if (CheckForClimbableLedge(WallLocation, LedgeLocation))
	{
		CharacterMovementComponent->ClimbLedge(GetActorLocation(), WallLocation, LedgeLocation);
	}
}


// Overrides of protected interface

void AVDCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AVDCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	CharacterMovementComponent = Cast<UVDCharacterMovementComponent>(GetMovementComponent());
}


// VD protected interface

void AVDCharacter::AdjustEyeHeight()
{
	float TopHeight = CharacterMovementComponent->GetCurrentHalfHeight();
	float EyeHeight = TopHeight - HeadToEyeHeight;
	GetViewAttachment()->SetRelativeLocation(FVector::UpVector * EyeHeight);
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
					i->OnUnfocused(this);
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
					i->OnFocused(this);
				}
			}
		}
	}
}

bool AVDCharacter::CheckForClimbableLedge(FVector& WallLocation, FVector& LedgeLocation)
{
	// Create collision shapes for the current state, and one explicitly for a crouching character
	FCollisionShape RegularShape = GetCapsuleComponent()->GetCollisionShape();
	FCollisionShape CrouchingShape = RegularShape;
	CrouchingShape.Capsule.HalfHeight = GetCharacterMovementComponent()->CrouchedHalfHeight;
	float RegularHalfHeight = RegularShape.Capsule.HalfHeight + RegularShape.Capsule.Radius;
	float CrouchingHalfHeight = CrouchingShape.Capsule.HalfHeight + CrouchingShape.Capsule.Radius;

	// Ignore character on collision test
	FCollisionQueryParams TraceParams = FCollisionQueryParams::DefaultQueryParam;
	TraceParams.AddIgnoredActor(this);

	// Calculate trace start and end
	FVector WallTraceStart = GetActorLocation();
	FRotator WallTraceOrientation(0.f, ViewAttachment->GetComponentRotation().Yaw, 0.f);
	FVector WallTraceOffset = WallTraceOrientation.RotateVector(FVector::ForwardVector * LedgeCastDistance);
	FVector WallTraceEnd = WallTraceStart + WallTraceOffset;

	// Trace using character capsule
	FHitResult WallHitResult;

	if (GetWorld()->SweepSingleByChannel(WallHitResult, WallTraceStart, WallTraceEnd, FQuat::Identity,
		ECollisionChannel::ECC_Pawn, RegularShape, TraceParams))
	{
		WallLocation = WallHitResult.Location;

		// Get offset along floor plane
		FVector FloorLocation = WallHitResult.ImpactPoint - WallHitResult.ImpactNormal * ClimbForwardDistance;
		FloorLocation.Z = 0.f;

		// Calculate floor height
		float FloorHeight = GetActorLocation().Z - RegularHalfHeight;

		// Calculate trace start and end
		FVector LedgeTraceStart = FloorLocation;
		LedgeTraceStart.Z = FloorHeight + MaxLedgeHeight + CrouchingHalfHeight;
		FVector LedgeTraceEnd = FloorLocation;
		LedgeTraceEnd.Z = FloorHeight + MinLedgeHeight + CrouchingHalfHeight;

		// Trace using character capsule shape
		FHitResult LedgeHitResult;

		// Debug draw the start of the ledge trace
		DrawDebugCapsule(GetWorld(), LedgeTraceStart, CrouchingShape.Capsule.HalfHeight,
			CrouchingShape.Capsule.Radius, FQuat::Identity, FColor::Green, false, 0.1f);

		if (GetWorld()->SweepSingleByChannel(LedgeHitResult, LedgeTraceStart, LedgeTraceEnd, FQuat::Identity,
			ECollisionChannel::ECC_Pawn, CrouchingShape, TraceParams))
		{
			// Draw a small sphere on the ledge hit
			DrawDebugSphere(GetWorld(), LedgeHitResult.ImpactPoint, 1.f, 16, FColor::Red, false, 0.1f);

			// Calculate the angle between a flat plane and the ledge candidate
			float SlopeDotProduct = FVector::DotProduct(FVector::UpVector, LedgeHitResult.ImpactNormal);
			float SlopeAngle = FMath::RadiansToDegrees(FMath::Acos(SlopeDotProduct));

			if (!LedgeHitResult.bStartPenetrating && SlopeAngle < MaxLedgeAngle)
			{
				// Debug draw where the player can stand
				DrawDebugCapsule(GetWorld(), LedgeHitResult.Location, CrouchingShape.Capsule.HalfHeight, 
					CrouchingShape.Capsule.Radius, FQuat::Identity, FColor::Blue, false, 0.1f);

				LedgeLocation = LedgeHitResult.Location;

				return true;
			}
		}
	}

	return false;
}
