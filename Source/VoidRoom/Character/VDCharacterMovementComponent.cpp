// Copyright 2019 Spencer Melnick


#include "VDCharacterMovementComponent.h"

#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"

#include "../VoidRoom.h"
#include "VDCharacter.h"

UVDCharacterMovementComponent::UVDCharacterMovementComponent()
    : Super()
{
    bWantsInitializeComponent = true;
}


// Public engine overrides

void UVDCharacterMovementComponent::InitializeComponent()
{
    VoidCharacterOwner = Cast<AVDCharacter>(GetOwner());

    if (VoidCharacterOwner != nullptr)
    {
        UE_LOG(LogVD, Error, TEXT("UVDCharacterMovementComponent %s is not owned by a VDCharacter"), *GetNameSafe(this));
    }
}

void UVDCharacterMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	// Check for a change in crouch state. Players toggle crouch by changing bWantsToCrouch.
    UpdateCrouch(false, DeltaSeconds);
}

void UVDCharacterMovementComponent::Crouch(bool bClientSimulation)
{
    // Don't do anything
}

void UVDCharacterMovementComponent::UnCrouch(bool bClientSimulation)
{
    // Don't do anything
}

float UVDCharacterMovementComponent::GetMaxSpeed() const
{
	switch(MovementMode)
	{
	case MOVE_Walking:
	case MOVE_NavWalking:
        // Ease between the crouching and regular walking speed based on our crouch height
		return UKismetMathLibrary::Ease(MaxWalkSpeed, MaxWalkSpeedCrouched, CrouchEaseAlpha, CrouchEaseFunction);
	case MOVE_Falling:
		return MaxWalkSpeed;
	case MOVE_Swimming:
		return MaxSwimSpeed;
	case MOVE_Flying:
		return MaxFlySpeed;
	case MOVE_Custom:
		return MaxCustomMovementSpeed;
	case MOVE_None:
	default:
		return 0.f;
	}
}



// Public interface

void UVDCharacterMovementComponent::UpdateCrouch(bool bClientSimulation, float DeltaSeconds)
{
	if (!HasValidData())
	{
		return;
	}

    bool bShouldCrouch = bWantsToCrouch && CanCrouchInCurrentState();

	// if (bClientSimulation && CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)
	// {
	// 	// restore collision size before crouching
	// 	ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
	// 	VoidCharacterOwner->GetCapsuleComponent()->SetCapsuleSize(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight());
	// 	bShrinkProxyCapsule = true;
	// }

    // Accumulate time in our ease function
    float DeltaEaseAlpha = DeltaSeconds / CrouchEaseTime;
    float NewEaseAlpha = CrouchEaseAlpha;

    // Increment if crouching
    if (bShouldCrouch)
    {
        NewEaseAlpha += DeltaEaseAlpha;
    }
    // Decrement if standing up
    else
    {
        NewEaseAlpha -= DeltaEaseAlpha;
    }
    // Clamp to alpha range
    NewEaseAlpha = FMath::Clamp(NewEaseAlpha, 0.f, 1.f);

    // Early exit if nothing changed
    if (NewEaseAlpha == CrouchEaseAlpha)
    {
        return;
    }

    // Get some information about the current capsule
    UCapsuleComponent* Capsule = VoidCharacterOwner->GetCapsuleComponent();
    float CurrentHalfHeight = GetCurrentHalfHeight();
    FCollisionShape CurrentCollisionShape = Capsule->GetCollisionShape();
    FVector CurrentLocation = Capsule->GetComponentLocation();

    // Calculate some information about the potential new capsule
    float NewHalfHeight = UKismetMathLibrary::Ease(VoidCharacterOwner->GetDefaultHalfHeight(), CrouchedHalfHeight, NewEaseAlpha, CrouchEaseFunction);
    FCollisionShape NewCollisionShape = FCollisionShape::MakeCapsule(CurrentCollisionShape.Capsule.Radius, NewHalfHeight - CurrentCollisionShape.Capsule.Radius);
    FVector NewLocation;

    // Right now we just expand from the top of the capsule if we're falling, but this could be changed
    bool bExpandFromTop = IsFalling();

    if (bExpandFromTop)
    {
        FVector TopLocation = CurrentLocation + FVector::UpVector * CurrentHalfHeight;
        NewLocation = TopLocation + FVector::DownVector * NewHalfHeight;
    }
    else
    {
        FVector BottomLocation = CurrentLocation + FVector::DownVector * CurrentHalfHeight;
        NewLocation = BottomLocation + FVector::UpVector * NewHalfHeight;
    }

    if (!bClientSimulation)
    {
        bool bCanExpand;

        // If we're shrinking, we don't really need to do a test
        if (NewHalfHeight < CurrentHalfHeight)
        {
            bCanExpand = true;
        }
        else
        // Otherwise test our new capsule
        {
            FCollisionQueryParams CapsuleParams(SCENE_QUERY_STAT(CrouchTrace), false, CharacterOwner);
            FCollisionResponseParams ResponseParam;
            InitCollisionParams(CapsuleParams, ResponseParam);

            bool bEncroached = GetWorld()->OverlapBlockingTestByChannel(NewLocation, FQuat::Identity,
                UpdatedComponent->GetCollisionObjectType(), NewCollisionShape, CapsuleParams, ResponseParam);

            bCanExpand = !bEncroached;
        }

        if (bCanExpand)
        {
            // Update ease alpha
            CrouchEaseAlpha = NewEaseAlpha;

            // Update capsule and move
            Capsule->SetCapsuleSize(NewCollisionShape.GetCapsuleRadius(), NewCollisionShape.GetCapsuleHalfHeight());
            UpdatedComponent->MoveComponent(NewLocation - CurrentLocation, UpdatedComponent->GetComponentRotation(),
                true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);

            bForceNextFloorCheck = true;

            // For now, consider the character crouched if they have even begun the transition
            CharacterOwner->bIsCrouched = (CrouchEaseAlpha >= 0.f);

            // Trigger events
            // Trigger a start crouch once we've fully reached the crouching position
            if (CrouchEaseAlpha >= 1.f)
            {
                float DefaultHalfHeight = CharacterOwner->GetDefaultHalfHeight();
                float HalfHeightDifference = NewHalfHeight - DefaultHalfHeight;

                CharacterOwner->OnStartCrouch(HalfHeightDifference, HalfHeightDifference);
            }
            // Trigger an end crouch once we've fully reached the standing position
            else if (CrouchEaseAlpha <= 0.f)
            {
                CharacterOwner->OnEndCrouch(0.f, 0.f);
            }
        }
    }

	AdjustProxyCapsuleSize();


	// CharacterOwner->OnStartCrouch( HalfHeightAdjust, ScaledHalfHeightAdjust );

	// // Don't smooth this change in mesh position
	// if (bClientSimulation && CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)
	// {
	// 	FNetworkPredictionData_Client_Character* ClientData = GetPredictionData_Client_Character();
	// 	if (ClientData && ClientData->MeshTranslationOffset.Z != 0.f)
	// 	{
	// 		ClientData->MeshTranslationOffset -= FVector(0.f, 0.f, MeshAdjust);
	// 		ClientData->OriginalMeshTranslationOffset = ClientData->MeshTranslationOffset;
	// 	}
	// }
}

float UVDCharacterMovementComponent::GetCurrentHalfHeight() const
{
    return UKismetMathLibrary::Ease(VoidCharacterOwner->GetDefaultHalfHeight(), CrouchedHalfHeight, CrouchEaseAlpha, CrouchEaseFunction);
}

float UVDCharacterMovementComponent::GetCurrentHeight() const
{
    return GetCurrentHalfHeight() * 2.f;
}
