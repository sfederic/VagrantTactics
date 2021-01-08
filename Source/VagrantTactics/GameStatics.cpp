// Fill out your copyright notice in the Description page of Project Settings.

#include "GameStatics.h"
#include "VagrantTacticsGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

AGridActor* GameStatics::GetActorAtGridIndex(int x, int y)
{
	return nullptr;
}

ABattleGrid* GameStatics::GetActiveBattleGrid()
{
	return nullptr;
}

UMainGameInstance* GameStatics::GetMainInstance(const UWorld* world)
{
	UGameInstance* instance = UGameplayStatics::GetGameInstance(world);
	if (instance)
	{
		UMainGameInstance* mainInstance = Cast<UMainGameInstance>(instance);
		if (mainInstance)
		{
			return mainInstance;
		}
	}

	return nullptr;
}

APlayerUnit* GameStatics::GetPlayer(const UWorld* world)
{
	APawn* playerPawn = UGameplayStatics::GetPlayerPawn(world, 0);
	if (playerPawn)
	{
		APlayerUnit* player = Cast<APlayerUnit>(playerPawn);
		if (player)
		{
			return player;
		}
	}

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


