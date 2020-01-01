// Copyright 2019 Spencer Melnick


#include "GenericToggleComponent.h"

#include "../VoidRoom.h"

void UGenericToggleComponent::BeginPlay()
{
    Super::BeginPlay();

    // Try to rebind a named actor in the sequence to one specified by this component
    if (bRebindActorInSequence)
    {
        RebindSequenceActor();
    }

    // Seek end of sequence if toggled at play start
    SeekStartOrEnd(bIsToggled);

    bIsPlaying = true;
}

void UGenericToggleComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.Property->GetNameCPP() == "bIsToggled")
    {
        // Only try to use the sequence player if the game is running
        if (bIsPlaying)
        {
            UpdateSequence();
        }
    }
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
        TArray<FMovieSceneBinding> SequenceBindings = SequenceActor->GetSequence()->GetMovieScene()->GetBindings();
        FMovieSceneObjectBindingID SequenceTargetBindingID;
        bool bFoundBinding = false;

        // Search through the sequence's bindings for one matching our desired actor name
        for (auto& i : SequenceBindings)
        {
            if (i.GetName() == RebindFromActorName)
            {
                SequenceTargetBindingID = FMovieSceneObjectBindingID(i.GetObjectGuid(), MovieSceneSequenceID::Root);
                bFoundBinding = true;
                break;
            }
        }

        // If we found a binding, apply our new actor to the existing binding
        if (bFoundBinding)
        {
            SequenceActor->SetBinding(SequenceTargetBindingID, TArray<AActor*>({ RebindToActor }));

            UE_LOG(LogVD, Display, TEXT("UGenericToggleComponent %s rebound actor %s in sequence to actor %s"), *GetNameSafe(this), *RebindFromActorName, *RebindToActor->GetName());
        }
        else
        {
            UE_LOG(LogVD, Warning, TEXT("UGenericToggleComponent %s cannot find a binding for actor %s"), *GetNameSafe(this), *RebindFromActorName);
        }
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

