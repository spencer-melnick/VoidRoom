// Copyright 2019 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "InteractiveActor.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Components/StaticMeshComponent.h" 
#include "Components/BoxComponent.h"
#include "PhysicsEngine/BodyInstance.h" 


#include "InteractiveDoor.generated.h"

/**
 * 
 */
UCLASS()
class VOIDROOM_API AInteractiveDoor : public AInteractiveActor
{
	GENERATED_BODY()

public:
	AInteractiveDoor(const FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaTime) override;

	void Open();
	void Close();
	void RotateToAngle(float Angle, float DeltaTime);

protected:
	virtual void OnInteract(ACharacter* Character) override;

private:

	bool TryToLock(float LockAngle);

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* DoorMesh;

	UPROPERTY(EditAnywhere)
	UPhysicsConstraintComponent* Hinge;

	UPROPERTY(EditAnywhere)
	USceneComponent* DoorRoot;

	UPROPERTY(EditAnywhere)
	USceneComponent* Door;

	UPROPERTY(EditAnywhere)
	USceneComponent* HitboxRoot;

	UPROPERTY(EditAnywhere)
	UBoxComponent* Hitbox;

	UPROPERTY(VisibleAnywhere, Category = "Behavior")
	bool bIsOpen = false;

	UPROPERTY(VisibleAnywhere, Category = "Behavior")
	bool bIsClosing = false;

	UPROPERTY(VisibleAnywhere, Category = "Behavior")
	bool bIsOpening = false;

	UPROPERTY(EditAnywhere, Category = "Behavior")
	float RotateSpeed = 3.0f;

	UPROPERTY(EditAnywhere, Category = "Behavior")
	float ClosedAngle = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Behavior")
	float OpenAngle = 90.0f;

	UPROPERTY(EditAnywhere, Category = "Behavior")
	float LockThreshold = 5.0f;

	float CurrentAngle;
	float StepAngle;
};
