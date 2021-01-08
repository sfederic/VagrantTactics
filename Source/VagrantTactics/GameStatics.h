#pragma once

#include "GridActor.h"
#include "BattleGrid.h"
#include "MainGameInstance.h"
#include "PlayerUnit.h"

class UWorld;

class GameStatics
{
public:
	static AGridActor* GetActorAtGridIndex(int x, int y);
	static ABattleGrid* GetActiveBattleGrid();
	static UMainGameInstance* GetMainInstance(const UWorld* world);
	static APlayerUnit* GetPlayer(const UWorld* world);
};
