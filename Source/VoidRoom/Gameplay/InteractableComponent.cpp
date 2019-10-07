// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableComponent.h"

#include "Components/MeshComponent.h"

// Sets default values for this component's properties
UInteractableComponent::UInteractableComponent()
{
	
}


// Called when the game starts
void UInteractableComponent::BeginPlay()
{
	Super::BeginPlay();

	ShowHighlight();
}


void UInteractableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UInteractableComponent::ShowHighlight()
{
	TInlineComponentArray<UMeshComponent*> MeshComponents;
	GetOwner()->GetComponents(MeshComponents);

	UE_LOG(LogTemp, Display, TEXT("Highlighting mesh"));

	for (auto& i : MeshComponents)
	{
		// Enable drawing to the stencil buffer with the correct value to render outlines
		i->bRenderCustomDepth = true;
		i->CustomDepthStencilWriteMask = ERendererStencilMask::ERSM_255;
		i->CustomDepthStencilValue = StencilOutlineValue;

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

		// Tell the engine to update the mesh state
		i->MarkRenderStateDirty();
	}
}

