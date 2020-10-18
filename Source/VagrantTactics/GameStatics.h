#pragma once

#include "Unit.h"
#include "BattleGrid.h"

class GameStatics
{
public:
	static AUnit* GetActorAtGridIndex(int x, int y);
	//static ABattleGrid* GetActiveBattleGrid();
};
