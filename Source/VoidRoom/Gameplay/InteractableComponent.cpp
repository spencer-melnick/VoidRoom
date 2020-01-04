// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableComponent.h"

#include "Components/MeshComponent.h"
#include "../VoidRoom.h"


// VD public interface

void UInteractableComponent::OnFocused()
{
	ShowHighlight();
}

void UInteractableComponent::OnUnfocused()
{
	HideHighlight();
}

void UInteractableComponent::OnInteract(AVDCharacter* Character)
{
	UE_LOG(LogVD, Log, TEXT("Character %s has interacted with incomplete interactable component %s"), *GetNameSafe(Character), *GetNameSafe(this));
}


// VD protected interface

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
