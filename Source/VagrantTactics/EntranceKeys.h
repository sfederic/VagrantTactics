#pragma once

#include "UObject/NameTypes.h"

//Lot of keys will be very specific to levels and doors due to story.
//Opted for strings because scared of having a 50+ enum list in editor.
namespace EntranceKeys
{
	static const FName TrapDoorKey = TEXT("TrapDoorKey");
}