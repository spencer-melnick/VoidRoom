// Copyright 2019 Spencer Melnick


#include "VDPlayerState.h"

#include "Net/UnrealNetwork.h"


void AVDPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AVDPlayerState, Inventory);
}

void AVDPlayerState::OnRep_Inventory() {
    // Do something here!
}
