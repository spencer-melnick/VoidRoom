// Copyright 2019 Spencer Melnick


#include "InteractiveActor.h"

#include "Components/MeshComponent.h"
#include "../VoidRoom.h"

AInteractiveActor::AInteractiveActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Interactive objects should replicate themselves
	bReplicates = true;
}



// VD public interfaces

void AInteractiveActor::LocalFocused(ACharacter* Character)
{
	OnLocalFocused(Character);
}

void AInteractiveActor::LocalUnfocused(ACharacter* Character)
{
	OnLocalUnfocused(Character);
}



// Networked functions

void AInteractiveActor::MulticastInteract_Implementation(ACharacter* Character)
{
	OnInteract(Character);
}



// VD protected interfaces

void AInteractiveActor::OnLocalFocused(ACharacter* Character)
{
	ShowHighlight();
}

void AInteractiveActor::OnLocalUnfocused(ACharacter* Character)
{
	HideHighlight();
}

void AInteractiveActor::OnInteract(ACharacter* Character)
{
	
}




// Helper functions

void AInteractiveActor::ShowHighlight()
{
	TInlineComponentArray<UMeshComponent*> MeshComponents;
	GetComponents(MeshComponents);

	for (auto& i : MeshComponents)
	{
		// Enable drawing to the stencil buffer with the correct value to render outlines
		i->bRenderCustomDepth = true;
		i->CustomDepthStencilWriteMask = ERendererStencilMask::ERSM_255;
		i->CustomDepthStencilValue |= VDGame::StencilOutlineFlag;

		// Tell the engine to update the mesh state
		i->MarkRenderStateDirty();
	}
}

void AInteractiveActor::HideHighlight()
{
	TInlineComponentArray<UMeshComponent*> MeshComponents;
	GetComponents(MeshComponents);

	for (auto& i : MeshComponents)
	{
		// Disable custom stencil rendering
		i->bRenderCustomDepth = false;
		i->CustomDepthStencilValue &= ~VDGame::StencilOutlineFlag;

		// Tell the engine to update the mesh state
		i->MarkRenderStateDirty();
	}
}
