// Fill out your copyright notice in the Description page of Project Settings.

#include "GameStatics.h"
#include "VagrantTacticsGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

AGridActor* GameStatics::GetActorAtGridIndex(int x, int y)
{
	return nullptr;
}

//TODO: gamemode constantly returning null. Had to go back to using older approach per actor
/*ABattleGrid* GameStatics::GetActiveBattleGrid()
{
	if (GEngine)
	{
		AVagrantTacticsGameModeBase* gameMode = Cast<AVagrantTacticsGameModeBase>(UGameplayStatics::GetGameMode(GEngine->GetWorld()));
		check(gameMode->activeBattleGrid);
		return gameMode->activeBattleGrid;
	}
	return nullptr;
}*/
