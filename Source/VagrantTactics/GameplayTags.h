#pragma once

#include "UObject/NameTypes.h"

namespace GameplayTags
{
	static const FName Player = TEXT("Player");
	static const FName Platform = TEXT("Platform"); //Tag for actors that displace nodes based on height
	static const FName Destructible = TEXT("Destructible");
	static const FName Fence = TEXT("Fence"); //Whether or not a wall in-game is acting as a barrier between grid nodes
}