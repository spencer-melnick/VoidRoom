// Copyright 2019 Spencer Melnick


#include "InteractiveToggle.h"

#include "../../VoidRoom.h"

AInteractiveToggle::AInteractiveToggle(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // Create a default scene component
    USceneComponent* SceneComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, "SceneComponent");
	RootComponent = SceneComponent;
}



// Public engine overrides

void AInteractiveToggle::BeginPlay()
{
    Super::BeginPlay();

    FMovieSceneSequencePlaybackSettings PlaybackSettings;
    PlaybackSettings.bAutoPlay = false;
    PlaybackSettings.bPauseAtEnd = true;

    UTemplateSequencePlayer::CreateTemplateSequencePlayer(GetWorld(), GetSequence(), PlaybackSettings, SequenceActor);

    if (SequenceActor != nullptr)
    {
        SequenceActor->SetBinding(this);
    }
}



// Helper functions

UTemplateSequence* AInteractiveToggle::GetSequence()
{
    if (!Sequence.IsValid())
    {
        return nullptr;
    }

    // Attempt to get sequence without loading
    UTemplateSequence* TemplateSequence = Cast<UTemplateSequence>(Sequence.ResolveObject());

    // Otherwise load
    if (TemplateSequence == nullptr)
    {
        TemplateSequence = Cast<UTemplateSequence>(Sequence.TryLoad());
    }

    return TemplateSequence;
}



// VD interface overrides

void AInteractiveToggle::OnInteract(ACharacter* Character)
{
    bIsToggled = !bIsToggled;

    if (SequenceActor != nullptr)
    {
        if (bIsToggled)
        {
            SequenceActor->SequencePlayer->Play();
        }
        else
        {
            SequenceActor->SequencePlayer->PlayReverse();
        }
    }
}
