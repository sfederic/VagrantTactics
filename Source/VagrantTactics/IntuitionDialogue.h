#pragma once

#include "UObject/NameTypes.h"
#include "IntuitionDialogue.generated.h"

//This struct will usually be placed in arrays of it in order for dialogue to be matched against
//various intuition ID's the player has.
USTRUCT(BlueprintType)
struct FIntuitionDialogue
{
	GENERATED_BODY()

	//Line of dialogue to happen on event
	UPROPERTY(EditAnywhere)
	FText dialogueLine;

	//Intuition ID to match against for dialogue to pop up
	UPROPERTY(EditAnywhere)
	FName intuitionID;
};