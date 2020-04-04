// Fill out your copyright notice in the Description page of Project Settings.


#include "VDCharacter.h"

#include "Math/UnrealMathUtility.h"
#include "Components/PrimitiveComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

#include "../VoidRoom.h"
#include "../Gameplay/Interactive/InteractiveActor.h"

AVDCharacter::AVDCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UVDCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame
	PrimaryActorTick.bCanEverTick = true;

	// Spawn view attachment
	ViewAttachment = CreateDefaultSubobject<USceneComponent>(TEXT("ViewAttachment"));
	ViewAttachment->SetupAttachment(GetRootComponent());

	//Spawn physical representation of view attachment
	LookRotator = CreateDefaultSubobject <USphereComponent>(TEXT("LookRotator"));
	LookRotator->SetupAttachment(ViewAttachment);

	//Spawn the constraint we use to pick up objects
	CarrierConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("CarrierConstraint"));
	CarrierConstraint->SetupAttachment(GetRootComponent());
	
	//Spawn a hitbox for use in attacks !!DEBUG ONLY!!
	DefaultAttackHitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("Default Attack Hitbox"));
	DefaultAttackHitbox->SetupAttachment(ViewAttachment);

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

bool AVDCharacter::CanJumpInternal_Implementation() const
{
	bool bCanJump = true;
	if (bCanJump)
	{
		// Ensure JumpHoldTime and JumpCount are valid.
		if (!bWasJumping || GetJumpMaxHoldTime() <= 0.0f)
		{
			if (JumpCurrentCount == 0 && GetCharacterMovement()->IsFalling())
			{
				bCanJump = JumpCurrentCount + 1 < JumpMaxCount;
			}
			else
			{
				bCanJump = JumpCurrentCount < JumpMaxCount;
			}
		}
		else
		{
			// Only consider JumpKeyHoldTime as long as:
			// A) The jump limit hasn't been met OR
			// B) The jump limit has been met AND we were already jumping
			const bool bJumpKeyHeld = (bPressedJump && JumpKeyHoldTime < GetJumpMaxHoldTime());
			bCanJump = bJumpKeyHeld &&
						((JumpCurrentCount < JumpMaxCount) || (bWasJumping && JumpCurrentCount == JumpMaxCount));
		}
	}
	return bCanJump;
}

void AVDCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AVDCharacter, LookPitch)
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

bool AVDCharacter::GetCanFocus() const
{
	return !bIsCarryingObject;
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
	if (bIsCarryingObject)
	{
		ServerDropObject();
	}
	else if (FocusedActor != nullptr)
	{
		ServerInteract(FocusedActor);
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

void AVDCharacter::Attack()
{
	if (bCanAttack)
	{
		TSet<AActor*> Targets;
		DefaultAttackHitbox->SetHiddenInGame(false, false);
		DefaultAttackHitbox->GetOverlappingActors(Targets);
		GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandle, this, &AVDCharacter::OnCooldownTimerEnd, AttackCooldown, false);
		for (AActor* Target : Targets)
		{
			if (Target != this)
			{
				UE_LOG(LogVD, Display, TEXT("Player hit %s with an attack!"), *GetNameSafe(Target))
			}
		}
	}
}

void AVDCharacter::DropListener(int32 ConstraintIndex)
{
	bIsCarryingObject = false;
	UE_LOG(LogVD, Warning, TEXT("%s's constraint was broken by force. ConstraintIndex = %d"), *GetNameSafe(this), ConstraintIndex);

}


// Overrides of protected interface

void AVDCharacter::BeginPlay()
{
	//Listen for event
	CarrierConstraint->OnConstraintBroken.AddDynamic(this, &AVDCharacter::DropListener);


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
	if (IsLocallyControlled())
	{
		FRotator ControlRotation = GetControlRotation();
		GetViewAttachment()->SetRelativeRotation(FRotator(ControlRotation.Pitch, 0.f, 0.f));
		ServerSetLookPitch(ControlRotation.Pitch);
	}
	else
	{
		GetViewAttachment()->SetRelativeRotation(FRotator(LookPitch, 0.f, 0.f));
	}
}

void AVDCharacter::CheckFocus()
{
	AActor* HitActor = nullptr;

	if (GetCanFocus())
	{
		// Setup line trace start and end
		FVector TraceStart = ViewAttachment->GetComponentLocation();
		FVector TraceOffset = ViewAttachment->GetComponentRotation().RotateVector(FVector::ForwardVector);
		TraceOffset *= MaxFocusDistance;
		FVector TraceEnd = TraceStart + TraceOffset;

		// Perform trace against visibility channel
		FHitResult HitResult;
		if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility))
		{
			HitActor = HitResult.GetActor();
		}
	}

	// If we're looking at a new actor
	if (FocusedActor != HitActor)
	{
		// Notify the previous focused actor that it was unfocused locally
		if (FocusedActor != nullptr)
		{
			AInteractiveActor* InteractiveActor = Cast<AInteractiveActor>(FocusedActor);

			if (InteractiveActor != nullptr)
			{
				InteractiveActor->LocalUnfocused(this);
			}
		}

		// Notify the new actor that it has been focused locally
		FocusedActor = HitActor;
		if (FocusedActor != nullptr)
		{
			AInteractiveActor* InteractiveActor = Cast<AInteractiveActor>(FocusedActor);

			if (InteractiveActor != nullptr)
			{
				InteractiveActor->LocalFocused(this);
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

		if (GetWorld()->SweepSingleByChannel(LedgeHitResult, LedgeTraceStart, LedgeTraceEnd, FQuat::Identity,
			ECollisionChannel::ECC_Pawn, CrouchingShape, TraceParams))
		{
			// Calculate the angle between a flat plane and the ledge candidate
			float SlopeDotProduct = FVector::DotProduct(FVector::UpVector, LedgeHitResult.ImpactNormal);
			float SlopeAngle = FMath::RadiansToDegrees(FMath::Acos(SlopeDotProduct));

			if (!LedgeHitResult.bStartPenetrating && SlopeAngle < MaxLedgeAngle)
			{
				LedgeLocation = LedgeHitResult.Location;

				return true;
			}
		}
	}

	return false;
}

void AVDCharacter::OnCooldownTimerEnd()
{
	GetWorld()->GetTimerManager().ClearTimer(CooldownTimerHandle);
	UE_LOG(LogVD, Display, TEXT("Equipment is ready to use again!"))
	DefaultAttackHitbox->SetHiddenInGame(true, false);
}

// Networked functions

bool AVDCharacter::ServerSetLookPitch_Validate(float NewPitch)
{
	return true;
}

void AVDCharacter::ServerSetLookPitch_Implementation(float NewPitch)
{
	LookPitch = NewPitch;
}

bool AVDCharacter::ServerInteract_Validate(AActor* Target)
{
	// TODO: Test if the target can actually be interacted with

	return true;
}

void AVDCharacter::ServerInteract_Implementation(AActor* Target)
{
	if (Target != nullptr)
	{
		AInteractiveActor* InteractiveActor = Cast<AInteractiveActor>(Target);

		if (InteractiveActor != nullptr)
		{
			if (InteractiveActor->ActorHasTag("PhysicsPickup"))
			{
				UE_LOG(LogVD, Display, TEXT("%s is carrying %s"), *GetNameSafe(this), *GetNameSafe(InteractiveActor));
				MulticastCarryObject(InteractiveActor);
			}
			else
			{
				InteractiveActor->ServerInteract(this);
			}
		}
	}
}

void AVDCharacter::MulticastCarryObject_Implementation(AInteractiveActor* Target)
{
	if (Target != nullptr)
	{
		UPrimitiveComponent* TargetComponent = FindComponentByClass<UPrimitiveComponent>();
		if (TargetComponent != nullptr)
		{
			CarrierConstraint->ConstraintActor1 = Target;
			CarrierConstraint->OverrideComponent2 = LookRotator;
			CarrierConstraint->InitComponentConstraint();
			
			CarrierConstraint->SetConstraintReferencePosition(EConstraintFrame::Frame1, FVector(0.0f));
			CarrierConstraint->SetConstraintReferencePosition(EConstraintFrame::Frame2, FVector::ForwardVector * CarryDistance);
			CarrierConstraint->UpdateConstraintFrames();

			bIsCarryingObject = true;
		}
	}
}

bool AVDCharacter::ServerDropObject_Validate()
{
	// TODO: Add some validation check

	return true;
}

void AVDCharacter::ServerDropObject_Implementation()
{
	MulticastDropObject();
}

void AVDCharacter::MulticastDropObject_Implementation()
{
	TInlineComponentArray<UPrimitiveComponent*> PrimitiveComponents;
	CarrierConstraint->ConstraintActor1->GetComponents(PrimitiveComponents);

	for (auto& PrimitiveComponent : PrimitiveComponents)
	{
		PrimitiveComponent->WakeAllRigidBodies();
	}
	
	CarrierConstraint->BreakConstraint();
	bIsCarryingObject = false;
}