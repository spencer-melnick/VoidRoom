// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableComponent.h"

#include "Components/MeshComponent.h"
#include "GameFramework/PlayerController.h"

#include "../Character/VDCharacter.h"
#include "../VoidRoom.h"


UInteractableComponent::UInteractableComponent()
{
	// Replicate this object and its owner
	SetIsReplicated(true);
	// GetOwner()->SetReplicates(true);
}

// VD public interface

void UInteractableComponent::OnFocused(AVDCharacter* Character)
{
	if (Character->IsLocallyControlled())
	{
		ShowHighlight();
	}
}

void UInteractableComponent::OnUnfocused(AVDCharacter* Character)
{
	if (Character->IsLocallyControlled())
	{
		HideHighlight();
	}
}


// Networked interact
bool UInteractableComponent::MulticastInteract_Validate(AVDCharacter* Character)
{
	// TODO: Some kind of validation? Depends on how much we trust the server
	return true;
}

void UInteractableComponent::MulticastInteract_Implementation(AVDCharacter* Character)
{
	OnInteract(Character);
}


// VD protected interface

void UInteractableComponent::OnInteract(AVDCharacter* Character)
{
	UE_LOG(LogVD, Log, TEXT("Character %s has interacted with incomplete interactable component %s"), *GetNameSafe(Character), *GetNameSafe(this));
}

void UInteractableComponent::ShowHighlight()
{
	TInlineComponentArray<UMeshComponent*> MeshComponents;
	GetOwner()->GetComponents(MeshComponents);

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

void UInteractableComponent::HideHighlight()
{
	TInlineComponentArray<UMeshComponent*> MeshComponents;
	GetOwner()->GetComponents(MeshComponents);

	for (auto& i : MeshComponents)
	{
		// Disable custom stencil rendering
		i->bRenderCustomDepth = false;
		i->CustomDepthStencilValue &= ~VDGame::StencilOutlineFlag;

		// Tell the engine to update the mesh state
		i->MarkRenderStateDirty();
	}
}
