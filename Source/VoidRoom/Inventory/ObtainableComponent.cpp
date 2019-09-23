// Fill out your copyright notice in the Description page of Project Settings.


#include "ObtainableComponent.h"

// Sets default values for this component's properties
UObtainableComponent::UObtainableComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UObtainableComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

//Logic that occurs when the object is picked up
//Returns the ItemID by default, which should be looked up in the gamemode.
int16 UObtainableComponent::Obtain(){
	Obtainable = false;
	return ItemID;
}


// Called every frame
void UObtainableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

