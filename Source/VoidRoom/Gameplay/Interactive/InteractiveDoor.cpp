// Copyright 2019 Spencer Melnick


#include "InteractiveDoor.h"

#include "../../VoidRoom.h"

AInteractiveDoor::AInteractiveDoor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DoorOpening = CreateDefaultSubobject<USceneComponent>(TEXT("Door Opening"));
	// DoorOpening->SetBoxExtent(FVector(50.0f, 25.0f, 100.0f));
	RootComponent = DoorOpening;

	Door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door Mesh"));
	Door->SetupAttachment(GetRootComponent());

	/*Hinge = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("Hinge"));
	Hinge->SetupAttachment(GetRootComponent());

	Hinge->SetRelativeRotation(FRotator(90.0f, 0.0f, 90.0f));
	Hinge->SetRelativeLocation(FVector(25.0f, 0.0f, 50.0f));

	Hinge->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Locked, 0.0f);
	Hinge->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Locked, 0.0f);
	Hinge->SetAngularTwistLimit(EAngularConstraintMotion::ACM_Limited, 80.0f);

	Hinge->OverrideComponent1 = Cast<UPrimitiveComponent>(GetRootComponent());
	Hinge->OverrideComponent2 = Cast<UPrimitiveComponent>(Door);*/

	PrimaryActorTick.bCanEverTick = true;
	//SetActorTickEnabled(true);
}

void AInteractiveDoor::BeginPlay()
{
	Super::BeginPlay();
	Door->OnComponentHit.AddDynamic(this, &AInteractiveDoor::RecieveHit);

	Door->SetupAttachment(RootComponent);
	UE_LOG(LogVD, Display, TEXT("Door's parent is %s, the root is %s"), *GetNameSafe(Door->GetAttachParent()), *GetNameSafe(GetRootComponent()));
	TArray<USceneComponent*> actorList;
	DoorOpening->GetChildrenComponents(true, actorList);

	for (USceneComponent* actor : actorList)
	{
		UE_LOG(LogVD, Display, TEXT("children: %s"), *actor->GetName());
	}

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
	if (bIsOpen)
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
	StepAngle = FMath::FInterpTo(CurrentAngle, Angle, DeltaTime, 1.0f);
	FRotator StepRotator = FRotator(0.0f, StepAngle, 0.0f);
	//UE_LOG(LogVD, Display, TEXT("Door is rotating by %f degrees, over %f seconds"), StepAngle, DeltaTime);
	FHitResult HitResult;
	Door->SetRelativeRotation(StepRotator, true, &HitResult, ETeleportType::None);
}

bool AInteractiveDoor::TryToLock(float LockAngle)
{
	UE_LOG(LogVD, Display, TEXT("Attempting to lock door, %f"), FGenericPlatformMath::Abs(CurrentAngle - LockAngle));
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

void AInteractiveDoor::RecieveHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult &HitResult)
{
	if (!bIsOpen)
	{
		//Hinge->SetAngularTwistLimit(EAngularConstraintMotion::ACM_Limited, Door->GetRelativeRotation().Yaw);
		//UE_LOG(LogVD, Warning, TEXT("A Door was hit!"));
	}
}