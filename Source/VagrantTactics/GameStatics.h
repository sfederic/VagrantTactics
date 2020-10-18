#pragma once

#include "GridActor.h"
#include "BattleGrid.h"

class GameStatics
{
public:
	static AGridActor* GetActorAtGridIndex(int x, int y);
	//static ABattleGrid* GetActiveBattleGrid();
};
