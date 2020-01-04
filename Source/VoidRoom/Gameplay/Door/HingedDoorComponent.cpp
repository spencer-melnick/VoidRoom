// Copyright 2019 Spencer Melnick


#include "HingedDoorComponent.h"

#include "Components/PrimitiveComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"

#include "../../VoidRoom.h"

UHingedDoorComponent::UHingedDoorComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UHingedDoorComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Scale alpha by transition time
    float DeltaAlpha = DeltaTime / TransitionTime;

    // Increase or decrease alpha if door is open or closed respectively
    if (bIsOpen)
    {
        TransitionAlpha += DeltaAlpha;
    }
    else
    {
        TransitionAlpha -= DeltaAlpha;
    }
    TransitionAlpha = FMath::Clamp(TransitionAlpha, 0.f, 1.f);

    // Enable or disable door collisions
    if (DoorComponent != nullptr)
    {
        if (TransitionAlpha > ClosedThreshold)
        {
            DoorComponent->BodyInstance.SetResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
            DoorComponent->BodyInstance.SetResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);
        }
        else
        {
            DoorComponent->BodyInstance.SetResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
            DoorComponent->BodyInstance.SetResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Block);
        }
    }
    
    // Ease rotation
    if (OriginComponent != nullptr)
    {
        float Angle = UKismetMathLibrary::Ease(ClosedAngle, OpenAngle, TransitionAlpha, EasingFunction);
        OriginComponent->SetRelativeRotation(FQuat(FVector::UpVector, FMath::DegreesToRadians(Angle)));
    }
}

void UHingedDoorComponent::BeginPlay()
{
    Super::BeginPlay();

    AActor* Actor = GetOwner();

    // Set initial transition alpha
    if (bIsOpen)
    {
        TransitionAlpha = 1.f;
    }
    else
    {
        TransitionAlpha = 0.f;
    }

    if (Actor != nullptr)
    {
        TSet<UActorComponent*> ActorComponents = Actor->GetComponents();

        // Find components by name
        for (auto i : ActorComponents)
        {
            USceneComponent* SceneComponent = Cast<USceneComponent>(i);
            UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(i);

            if (SceneComponent != nullptr && SceneComponent->GetName() == OriginComponentName)
            {
                // Find axis origin component
                OriginComponent = SceneComponent;
            }
            else if (PrimitiveComponent != nullptr && PrimitiveComponent->GetName() == DoorComponentName)
            {
                // Find door component
                DoorComponent = PrimitiveComponent;
            }

            // If both are found exit loop early
            if (OriginComponent != nullptr && DoorComponent != nullptr)
            {
                break;
            }
        }
    }
}

void UHingedDoorComponent::OnInteract(AVDCharacter* Character)
{
    bIsOpen = !bIsOpen;
}

float UHingedDoorComponent::GetDesiredAngle() const
{
    if (bIsOpen)
    {
        return OpenAngle;
    }
    else
    {
        return ClosedAngle;
    }
}
