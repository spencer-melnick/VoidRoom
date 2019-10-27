// Copyright 2019 Spencer Melnick


#include "HingedDoorComponent.h"

#include "Components/PrimitiveComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"

void UHingedDoorComponent::BeginPlay()
{
    UpdateHinge();
}

void UHingedDoorComponent::OnInteract(AVDCharacter* Character)
{
    bIsOpen = !bIsOpen;
    UpdateHinge();
}

void UHingedDoorComponent::UpdateHinge()
{
    if (bIsOpen)
    {
        SetAngle(OpenAngle);
    }
    else
    {
        SetAngle(ClosedAngle);
    }
}

void UHingedDoorComponent::SetAngle(float Angle)
{
    UPhysicsConstraintComponent* HingeConstraint = GetOwner()->FindComponentByClass<UPhysicsConstraintComponent>();
    UPrimitiveComponent* PhysicsComponent = GetOwner()->FindComponentByClass<UPrimitiveComponent>();

    if (HingeConstraint != nullptr && PhysicsComponent != nullptr)
    {
        HingeConstraint->SetAngularOrientationTarget(FRotator(0.f, Angle, 0.f));
        PhysicsComponent->WakeRigidBody();
    }
}
