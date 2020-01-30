// Copyright 2019 Spencer Melnick

#pragma once

#include "CoreMinimal.h"
#include "InteractiveActor.h"
#include "TemplateSequenceActor.h"
#include "TemplateSequence.h"
#include "InteractiveToggle.generated.h"


// Forward declare character
class ACharacter;

/**
 * 
 */
UCLASS()
class VOIDROOM_API AInteractiveToggle : public AInteractiveActor
{
	GENERATED_BODY()

public:
	AInteractiveToggle(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;

	virtual void OnInteract(ACharacter* Character) override;
	
	UTemplateSequence* GetSequence();

private:
	UPROPERTY(EditAnywhere, Category=Animation, meta = (AllowedClasses = "TemplateSequence"))
	FSoftObjectPath Sequence;

	UPROPERTY(Transient)
	ATemplateSequenceActor* SequenceActor;

	bool bIsToggled = false;
	
};
