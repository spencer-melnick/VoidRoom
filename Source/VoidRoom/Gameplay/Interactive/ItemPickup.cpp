// Copyright 2019 Spencer Melnick


#include "ItemPickup.h"

#include "../../VoidRoom.h"
#include "../../Character/VDCharacter.h"
#include "../../Controller/VDPlayerController.h"
#include "../../Player/VDPlayerState.h"


void AItemPickup::OnInteract(ACharacter* Character)
{
    AVDPlayerController* Controller = Cast<AVDPlayerController>(Character->GetController());

    if (Controller != nullptr)
    {
        AVDPlayerState* PlayerState = Controller->GetPlayerState<AVDPlayerState>();

        if (PlayerState != nullptr)
        {
            FInventoryObject PickupItem = *Object.GetRow<FInventoryObject>(*GetNameSafe(this));
            PlayerState->TryPickupObject(PickupItem);

            UE_LOG(LogVD, Display, TEXT("%s picked up a %s"), *Character->GetName(), *PickupItem.Name);
        }
    }
}

