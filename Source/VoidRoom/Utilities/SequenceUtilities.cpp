// Copyright 2019 Spencer Melnick

#include "SequenceUtilities.h"

#include "../VoidRoom.h"

void SequenceUtilities::RebindActorInSequence(ALevelSequenceActor* SequenceActor, FString FromActorName, AActor* ToActor)
{
    TArray<FMovieSceneBinding> SequenceBindings = SequenceActor->GetSequence()->GetMovieScene()->GetBindings();
    FMovieSceneObjectBindingID SequenceTargetBindingID;
    bool bFoundBinding = false;

    // Search through the sequence's bindings for one matching our desired actor name
    for (auto& i : SequenceBindings)
    {
        if (i.GetName() == FromActorName)
        {
            SequenceTargetBindingID = FMovieSceneObjectBindingID(i.GetObjectGuid(), MovieSceneSequenceID::Root);
            bFoundBinding = true;
            break;
        }
    }

    // If we found a binding, apply our new actor to the existing binding
    if (bFoundBinding)
    {
        SequenceActor->SetBinding(SequenceTargetBindingID, TArray<AActor*>({ ToActor }));

        UE_LOG(LogVD, Display, TEXT("Rebound actor %s in sequence to actor %s"), *FromActorName, *ToActor->GetName());
    }
    else
    {
        UE_LOG(LogVD, Warning, TEXT("Cannot find a binding for actor %s"), *FromActorName);
    }
}