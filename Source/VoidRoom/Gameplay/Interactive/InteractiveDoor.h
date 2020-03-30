// Copyright 2019 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "InteractiveActor.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Components/StaticMeshComponent.h" 
#include "Components/BoxComponent.h" 


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

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Dimensions;

	void Open();
	void Close();
	void RotateToAngle(float Angle, float DeltaTime);

	UFUNCTION()
	void RecieveHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse,const FHitResult &HitResult);

protected:
	virtual void OnInteract(ACharacter* Character) override;

private:

	bool TryToLock(float LockAngle);

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Door;

	UPROPERTY(EditAnywhere)
	UPhysicsConstraintComponent* Hinge;

	UPROPERTY(EditAnywhere)
	USceneComponent* DoorOpening;

	UPROPERTY(VisibleAnywhere, Category = "Behavior")
	bool bIsOpen = false;

	UPROPERTY(VisibleAnywhere, Category = "Behavior")
	bool bIsClosing = false;

	UPROPERTY(VisibleAnywhere, Category = "Behavior")
	bool bIsOpening = true;

	UPROPERTY(EditAnywhere, Category = "Behavior")
	float ClosedAngle = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Behavior")
	float OpenAngle = 90.0f;

	UPROPERTY(EditAnywhere, Category = "Behavior")
	float LockThreshold = 5.0f;

	float CurrentAngle;
	float StepAngle;
};
