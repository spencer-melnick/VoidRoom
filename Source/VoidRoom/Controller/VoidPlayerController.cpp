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
    ACharacter* PossessedPawn = Cast<ACharacter>(GetPawn());

    if (PossessedPawn)
    {
        FVector RayStart = PossessedPawn->GetTransform().GetLocation();
        FVector RayDir = GetControlRotation().RotateVector(FVector::ForwardVector);
        FVector RayEnd = RayStart + RayDir * FocusRange;

        FName TraceTag("DebugTrace");
        GetWorld()->DebugDrawTraceTag = TraceTag;
        
        FHitResult TraceResult;
        FCollisionQueryParams TraceParams;
        TraceParams.TraceTag = TraceTag;

        if (GetWorld()->LineTraceSingleByChannel(TraceResult, RayStart, RayEnd, ECollisionChannel::ECC_Visibility, TraceParams))
        {
            AActor* TracedActor = TraceResult.GetActor();

            if (FocusedActor != TracedActor)
            {
                if (FocusedActor != nullptr)
                {
                    EndFocusOnActor(FocusedActor);
                }

                FocusOnActor(TracedActor);
                FocusedActor = TracedActor;
            }
        }
        else
        {
            if (FocusedActor != nullptr)
            {
                EndFocusOnActor(FocusedActor);
                FocusedActor = nullptr;
            }
        }
    }
}

void AVoidPlayerController::FocusOnActor(AActor* Target)
{
    if (Target == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Attempting to end focus on null actor"));
    }
    else
    {
        TInlineComponentArray<UInteractableComponent*> InteractableComponents;
        Target->GetComponents<UInteractableComponent>(InteractableComponents, false);

        for (auto& i : InteractableComponents)
        {
            i->OnBecomeFocus();
        }
    }
}

void AVoidPlayerController::EndFocusOnActor(AActor* Target)
{
    if (Target == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Attempting to end focus on null actor"));
    }
    else
    {
        TInlineComponentArray<UInteractableComponent*> InteractableComponents;
        Target->GetComponents<UInteractableComponent>(InteractableComponents, false);

        for (auto& i : InteractableComponents)
        {
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

