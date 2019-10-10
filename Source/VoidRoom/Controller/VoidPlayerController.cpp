// Fill out your copyright notice in the Description page of Project Settings.


#include "VoidPlayerController.h"

#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "../Gameplay/InteractableComponent.h"


AVoidPlayerController::AVoidPlayerController()
{
    // Tick every frame
	PrimaryActorTick.bCanEverTick = true;
}

void AVoidPlayerController::MoveForward(float Scale)
{
    APawn* PossessedPawn = GetPawn();

    if (PossessedPawn)
    {
        // Make pawn movement component move
        PossessedPawn->AddMovementInput(PossessedPawn->GetActorForwardVector() * Scale);
    }
}

void AVoidPlayerController::MoveRight(float Scale)
{
    APawn* PossessedPawn = GetPawn();

    if (PossessedPawn)
    {
        // Make pawn movement component move
        PossessedPawn->AddMovementInput(PossessedPawn->GetActorRightVector() * Scale);
    }
}

void AVoidPlayerController::Crouch()
{
    ACharacter* PossessedPawn = Cast<ACharacter>(GetPawn());

    if (PossessedPawn)
    {
        PossessedPawn->Crouch();
    }
}

void AVoidPlayerController::UnCrouch()
{
    ACharacter* PossessedPawn = Cast<ACharacter>(GetPawn());

    if (PossessedPawn)
    {
        PossessedPawn->UnCrouch();
    }
}

void AVoidPlayerController::CheckFocus()
{
    // Get the look ray from the camera point of view
    FVector RayStart = PlayerCameraManager->GetCameraLocation();
    FVector RayDir = PlayerCameraManager->GetCameraRotation().RotateVector(FVector::ForwardVector);
    FVector RayEnd = RayStart + RayDir * FocusRange;

    // Perform a line trace against all visible objects
    FHitResult TraceResult;
    bool bDidHitTrace = GetWorld()->LineTraceSingleByChannel(TraceResult, RayStart, RayEnd, ECollisionChannel::ECC_Visibility);
    AActor* TracedActor = TraceResult.GetActor();

    // Consider a nullptr for traced actor as a failed hit
    if (bDidHitTrace && TracedActor != nullptr)
    {
        // If a new actor is in focus, trigger events
        if (FocusedActor != TracedActor)
        {
            EndFocusOnActor(FocusedActor);
            FocusOnActor(TracedActor);
            FocusedActor = TracedActor;
        }
    }
    else
    {
        // If no actor is traced, unfocus on exisiting factor
        EndFocusOnActor(FocusedActor);
        FocusedActor = nullptr;
    }
}

void AVoidPlayerController::FocusOnActor(AActor* Target)
{
    // Check for valid target
    if (Target != nullptr)
    {
        // Get all InteractableComponents (if any) in the target
        TInlineComponentArray<UInteractableComponent*> InteractableComponents;
        Target->GetComponents<UInteractableComponent>(InteractableComponents, false);

        for (auto& i : InteractableComponents)
        {
            // Trigger focus events
            i->OnBecomeFocus();
        }
    }
}

void AVoidPlayerController::EndFocusOnActor(AActor* Target)
{
    // Check for valid target
    if (Target != nullptr)
    {
        // Get all InteractableComponents (if any) in the target
        TInlineComponentArray<UInteractableComponent*> InteractableComponents;
        Target->GetComponents<UInteractableComponent>(InteractableComponents, false);

        for (auto& i : InteractableComponents)
        {
            // Trigger focus events
            i->OnEndFocus();
        }
    }
}

void AVoidPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    // Bind axes to movement functions
    InputComponent->BindAxis("MoveForward", this, &AVoidPlayerController::MoveForward);
    InputComponent->BindAxis("MoveRight", this, &AVoidPlayerController::MoveRight);

    InputComponent->BindAxis("LookUp", this, &AVoidPlayerController::AddPitchInput);
    InputComponent->BindAxis("LookRight", this, &AVoidPlayerController::AddYawInput);

    // Bind actions
    InputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &AVoidPlayerController::Crouch);
    InputComponent->BindAction("Crouch", EInputEvent::IE_Released, this, &AVoidPlayerController::UnCrouch);
}

void AVoidPlayerController::Tick(float DeltaTime)
{
    CheckFocus();
}

