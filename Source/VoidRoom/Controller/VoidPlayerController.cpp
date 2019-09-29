// Fill out your copyright notice in the Description page of Project Settings.


#include "VoidPlayerController.h"

#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"


AVoidPlayerController::AVoidPlayerController()
{

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

