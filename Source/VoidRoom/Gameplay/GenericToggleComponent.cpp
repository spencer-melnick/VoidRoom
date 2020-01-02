// Copyright 2019 Spencer Melnick


#include "GenericToggleComponent.h"

#include "../VoidRoom.h"
#include "../Utilities/SequenceUtilities.h"

void UGenericToggleComponent::BeginPlay()
{
    Super::BeginPlay();

    bIsPlaying = true;

    if (SequenceActor != nullptr)
    {
        // Automatically set pause at end (helps make animations reversible)
        SequenceActor->PlaybackSettings.bPauseAtEnd = true;
    }

    // Try to rebind a named actor in the sequence to one specified by this component
    if (bRebindActorInSequence)
    {
        RebindSequenceActor();
    }

    // Seek end of sequence if toggled at play start
    SeekStartOrEnd(bIsToggled);
}

void UGenericToggleComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    // if (PropertyChangedEvent.Property->GetNameCPP() == "bIsToggled")
    // {
    //     // Only try to use the sequence player if the game is running
    //     if (bIsPlaying)
    //     {
    //         UpdateSequence();
    //     }
    // }
}

void UGenericToggleComponent::OnInteract(AVDCharacter* Character)
{
    // Flip toggle state
    SetToggled(!GetIsToggled());
}

void UGenericToggleComponent::SetToggled(bool bToggled)
{
    if (bToggled != bIsToggled)
    {
        // Update sequence if the new toggle state is different
        bIsToggled = bToggled;
        UpdateSequence();
    }
}

bool UGenericToggleComponent::GetIsToggled() const
{
    return bIsToggled;
}

void UGenericToggleComponent::RebindSequenceActor()
{
    if (SequenceActor != nullptr && RebindToActor != nullptr)
    {
        SequenceUtilities::RebindActorInSequence(SequenceActor, RebindFromActorName, RebindToActor);
    }
}

void UGenericToggleComponent::UpdateSequence()
{
    if (SequenceActor != nullptr)
    {
        // When toggled, play forwards
        if (bIsToggled)
        {
            SequenceActor->GetSequencePlayer()->Play();
        }
        // Otherwise play backwards
        else
        {
            SequenceActor->GetSequencePlayer()->PlayReverse();
        }
    }
}

void UGenericToggleComponent::SeekStartOrEnd(bool bSeekEnd)
{
    if (SequenceActor != nullptr)
    {
        ULevelSequencePlayer* Player = SequenceActor->GetSequencePlayer();

        if (Player != nullptr)
        {
            if (bSeekEnd)
            {
                // Jump to the last frame
                Player->JumpToFrame(Player->GetFrameDuration() - 1);
            }
            else
            {
                // Jump to the first frame
                Player->JumpToFrame(0);
            }
        }
    }
}

