// Copyright 2019 Spencer Melnick


#include "InteractiveDoor.h"

#include "../../VoidRoom.h"

AInteractiveDoor::AInteractiveDoor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DoorRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Door Root"));
	RootComponent = DoorRoot;

	Door = CreateDefaultSubobject<USceneComponent>(TEXT("Door"));
	Door->SetupAttachment(GetRootComponent());

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door Mesh"));
	DoorMesh->SetupAttachment(Door);

	HitboxRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Hitbox Root"));
	HitboxRoot->SetupAttachment(GetRootComponent());

	PrimaryActorTick.bCanEverTick = true;

	Hitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("Hitbox"));
	Hitbox->SetupAttachment(HitboxRoot);
}

void AInteractiveDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsClosing)
	{
		RotateToAngle(ClosedAngle, DeltaTime);
		if (TryToLock(ClosedAngle)) {
			bIsClosing = false;
			bIsOpen = false;
		}
	}
	else if (bIsOpening)
	{
		RotateToAngle(OpenAngle, DeltaTime);
		if (TryToLock(OpenAngle)) {
			bIsOpening = false;
			bIsOpen = true;
		}
	}
}


void AInteractiveDoor::OnInteract(ACharacter* Character)
{
	if (bIsOpening)
	{
		bIsOpening = false;
		bIsClosing = true;
	}
	else if (bIsClosing)
	{
		bIsClosing = false;
		bIsOpening = true;
	}
	else if (bIsOpen)
	{
		Close();
	}
	else
	{
		Open();
	}

}

void AInteractiveDoor::Close()
{
	Door->ComponentTags.Add(TEXT("PushCharacter"));
	bIsClosing = true;
}

void AInteractiveDoor::Open()
{
	Door->ComponentTags.Remove(TEXT("PushCharacter"));
	bIsOpening = true;
}

void AInteractiveDoor::RotateToAngle(float Angle, float DeltaTime)
{
	CurrentAngle = Door->GetRelativeRotation().Yaw;
	StepAngle = FMath::FInterpTo(CurrentAngle, Angle, DeltaTime, RotateSpeed);
	FRotator StepRotator = FRotator(0.0f, StepAngle, 0.0f);

	FHitResult HitResult;
	FCollisionShape CollisionShape;

	HitboxRoot->SetRelativeRotation(StepRotator, true, &HitResult, ETeleportType::None);

	TArray<UPrimitiveComponent*>Collisions;
	Hitbox->GetOverlappingComponents(Collisions);
	if (Collisions.Num() == 0)
	{
		Door->SetRelativeRotation(StepRotator, true, &HitResult, ETeleportType::None);
	}
}

bool AInteractiveDoor::TryToLock(float LockAngle)
{
	if (FGenericPlatformMath::Abs(CurrentAngle - LockAngle) <= LockThreshold)
	{
		FHitResult HitResult;
		FRotator StepRotator = FRotator(0.0f, LockAngle, 0.0f);
		Door->SetRelativeRotation(FQuat(StepRotator), false, &HitResult, ETeleportType::TeleportPhysics);
		Door->SetRelativeLocation(FVector(0.0f));
		return true;
	}
	return false;
}