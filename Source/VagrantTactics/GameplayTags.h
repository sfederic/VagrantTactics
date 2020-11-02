#pragma once

#include "UObject/NameTypes.h"

//General tags
namespace GameplayTags
{
	static const FName Player = TEXT("Player");
	static const FName Platform = TEXT("Platform"); //Tag for actors that displace nodes based on height
	static const FName Unit = TEXT("Unit"); //Any NPC or enemy in the game
	static const FName Destructible = TEXT("Destructible");
	static const FName Fence = TEXT("Fence"); //Whether or not a wall in-game is acting as a barrier between grid nodes
	static const FName NonObstruct = TEXT("NonObstruct"); //Static mesh that will let a node be placed (grass, fence, torch)
}

//Tags for items and actors units can focus on
namespace ItemTags
{
	static const FName Burning = TEXT("Burning"); //Causing units to flee
}