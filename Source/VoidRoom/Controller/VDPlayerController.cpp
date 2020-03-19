// Fill out your copyright notice in the Description page of Project Settings.


#include "VDPlayerController.h"

#include "GameFramework/Pawn.h"

#include "../Character/VDCharacter.h"


// Override public interface

void AVDPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    // Bind axes to movement
    InputComponent->BindAxis("MoveForward", this, &AVDPlayerController::MoveForward);
    InputComponent->BindAxis("MoveRight", this, &AVDPlayerController::MoveRight);

    // Bind axes to look
    InputComponent->BindAxis("LookUp", this, &AVDPlayerController::AddPitchInput);
    InputComponent->BindAxis("LookRight", this, &AVDPlayerController::AddYawInput);

    // Bind actions
    InputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &AVDPlayerController::Crouch);
    InputComponent->BindAction("Crouch", EInputEvent::IE_Released, this, &AVDPlayerController::UnCrouch);
    InputComponent->BindAction("PrimaryAction", EInputEvent::IE_Pressed, this, &AVDPlayerController::Interact);
    InputComponent->BindAction("Climb", EInputEvent::IE_Pressed, this, &AVDPlayerController::Climb);
}


void AVDPlayerController::BeginPlay()
{
    Super::BeginPlay();

	UIWidget = CreateWidget<UUserWidget>(this, UIClass);

	if (UIWidget != nullptr)
	{
        UIWidget->AddToPlayerScreen(1);
	}
}


// Protected VD interface

void AVDPlayerController::MoveForward(float Scale)
{
    APawn* PossessedPawn = GetPawn();

    if (PossessedPawn)
    {
        PossessedPawn->AddMovementInput(PossessedPawn->GetActorForwardVector() * Scale);
    }
}

void AVDPlayerController::MoveRight(float Scale)
{
    APawn* PossessedPawn = GetPawn();

    if (PossessedPawn)
    {
        PossessedPawn->AddMovementInput(PossessedPawn->GetActorRightVector() * Scale);
    }
}

void AVDPlayerController::Crouch()
{
    AVDCharacter* PossessedPawn = Cast<AVDCharacter>(GetPawn());

    if (PossessedPawn)
    {
        PossessedPawn->StartCrouch();
    }
}

void AVDPlayerController::UnCrouch()
{
    AVDCharacter* PossessedPawn = Cast<AVDCharacter>(GetPawn());

    if (PossessedPawn)
    {
        PossessedPawn->StartUncrouch();
    }
}

void AVDPlayerController::Interact()
{
    AVDCharacter* PossessedPawn = Cast<AVDCharacter>(GetPawn());

    if (PossessedPawn)
    {
        PossessedPawn->Interact();
    }
}

void AVDPlayerController::Climb()
{
    AVDCharacter* PossessedPawn = Cast<AVDCharacter>(GetPawn());

    if (PossessedPawn != nullptr)
    {
        PossessedPawn->TryClimbLedge();
    }
}
