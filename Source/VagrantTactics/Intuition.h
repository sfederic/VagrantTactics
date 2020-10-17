#pragma once

#include "UObject/NameTypes.h"
#include "Intuition.generated.h"

//Intuitions are story based effects where the player has an intimate knowledge about something.
USTRUCT()
struct FIntuition
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere) FName name;
};
