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
            UInventoryObject* PickupItem = NewObject<UInventoryObject>();
            PickupItem->Name = PickupName;

            PlayerState->Inventory.Add(PickupItem);

            UE_LOG(LogVD, Display, TEXT("%s picked up a %s"), *Character->GetName(), *PickupName);
        }
    }
}

