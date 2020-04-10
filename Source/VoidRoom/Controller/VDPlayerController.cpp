// Fill out your copyright notice in the Description page of Project Settings.


#include "VDPlayerController.h"

#include "GameFramework/Pawn.h"
#include "Blueprint/UserWidget.h"

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
    InputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &AVDPlayerController::Jump);
    InputComponent->BindAction("Jump", EInputEvent::IE_Released, this, &AVDPlayerController::EndJump);

    InputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &AVDPlayerController::Crouch);
    InputComponent->BindAction("Crouch", EInputEvent::IE_Released, this, &AVDPlayerController::UnCrouch);

    InputComponent->BindAction("Interact", EInputEvent::IE_Pressed, this, &AVDPlayerController::Interact);
    //InputComponent->BindAction("Climb", EInputEvent::IE_Pressed, this, &AVDPlayerController::Climb);
    InputComponent->BindAction("PrimaryAction", EInputEvent::IE_Pressed, this, &AVDPlayerController::PrimaryAction);

    InputComponent->BindAction("ToggleInventory", EInputEvent::IE_Pressed, this, &AVDPlayerController::ToggleInventory);
}


void AVDPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (IsLocalPlayerController())
    {
        CreateUIWidgets();
        HideUIWidgets();
    }
}


// Public functions

void AVDPlayerController::SetControlState(EControlState NewControlState)
{
    HideUIWidgets();
    ControlState = NewControlState;

	// Switch statement to set input mode and show mouse cursor
	switch(ControlState)
	{
		case EControlState::GameControl:
            bShowMouseCursor = false;
            SetInputMode(FInputModeGameOnly());
            break;

        case EControlState::MenuControl:
        case EControlState::InventoryControl:
            bShowMouseCursor = true;
            SetInputMode(FInputModeGameAndUI());

			// Center the mouse in the viewport
            {
                int ViewportWidth, ViewportHeight;
                GetViewportSize(ViewportWidth, ViewportHeight);
                SetMouseLocation(ViewportWidth / 2, ViewportHeight / 2);
            }
            break;

        default:
            break;
	}

	// Separate switch statement to set visible widgets
	switch(ControlState)
	{
		case EControlState::InventoryControl:
            InventoryGridWidget->SetVisibility(ESlateVisibility::Visible);
            break;

        default:
            break;
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

void AVDPlayerController::Jump()
{
    AVDCharacter* PossessedPawn = Cast<AVDCharacter>(GetPawn());

    if (PossessedPawn)
    {
        PossessedPawn->Jump();
    }
}

void AVDPlayerController::EndJump()
{
    AVDCharacter* PossessedPawn = Cast<AVDCharacter>(GetPawn());

    if (PossessedPawn)
    {
        PossessedPawn->StopJumping();
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

void AVDPlayerController::PrimaryAction()
{
    AVDCharacter* PossessedPawn = Cast<AVDCharacter>(GetPawn());

    if (PossessedPawn != nullptr)
    {
        PossessedPawn->UseTool();
    }
}

void AVDPlayerController::ToggleInventory()
{
	if (ControlState == EControlState::GameControl)
	{
        SetControlState(EControlState::InventoryControl);
	}
    else if (ControlState == EControlState::InventoryControl)
    {
        SetControlState(EControlState::GameControl);
    }
}



// Private functions
void AVDPlayerController::CreateUIWidgets()
{
    InventoryGridWidget = CreateWidget<UUserWidget>(this, InventoryGridClass);

	if (InventoryGridWidget != nullptr)
	{
        InventoryGridWidget->AddToPlayerScreen();
	}
}

void AVDPlayerController::HideUIWidgets()
{
    InventoryGridWidget->SetVisibility(ESlateVisibility::Hidden);
}


