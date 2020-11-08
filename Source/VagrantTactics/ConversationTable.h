#pragma once

#include "Engine/DataTable.h" 
#include "ConversationTable.generated.h"

class ANPCUnit;

USTRUCT(Blueprintable)
struct FConversationTable : public FTableRowBase
{
	GENERATED_BODY()
public:
	//Name of character saying the line
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText speakerName;

	//Line of dialogue from above character
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText dialogueLine;
};