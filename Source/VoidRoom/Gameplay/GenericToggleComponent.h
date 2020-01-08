// Copyright 2019 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "LevelSequenceActor.h"

#include "InteractableComponent.h"

#include "GenericToggleComponent.generated.h"


UCLASS( ClassGroup=(VoidRoom), meta=(BlueprintSpawnableComponent) )
class VOIDROOM_API UGenericToggleComponent : public UInteractableComponent
{
	GENERATED_BODY()

public:
	// Engine events
	virtual void BeginPlay() override;

	// VoidRoom events
	virtual void OnInteract(AVDCharacter* Character) override;

	// Public interface
	void SetToggled(bool bToggled);
	bool GetIsToggled() const;

	// Properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	ALevelSequenceActor* SequenceActor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	bool bRebindActorInSequence = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation, meta=(EditCondition="bRebindActorInSequence"))
	FString RebindFromActorName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation, meta=(EditCondition="bRebindActorInSequence"))
	AActor* RebindToActor;

protected:
	void RebindSequenceActor();
	void UpdateSequence();
	void SeekStartOrEnd(bool bSeekEnd);

private:
	// Private properties
	UPROPERTY(EditAnywhere, Category = Animation)
	bool bIsToggled = false;
	bool bIsPlaying = false;
};
