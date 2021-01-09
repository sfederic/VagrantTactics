// Fill out your copyright notice in the Description page of Project Settings.

#include "VagrantTacticsGameModeBase.h"
#include "GridActor.h"
#include "Kismet/GameplayStatics.h"
#include "BattleGrid.h"
#include "Unit.h"
#include "PlayerUnit.h"
#include "GameStatics.h"

AVagrantTacticsGameModeBase::AVagrantTacticsGameModeBase()
{

}

void AVagrantTacticsGameModeBase::BeginPlay()
{
	Super::BeginPlay();	

	//Get BattleGrid
	TArray<AActor*> outBattleGrid;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABattleGrid::StaticClass(), outBattleGrid);
	check(outBattleGrid.Num() == 1); //Make sure there's only one grid per level
	if (outBattleGrid.Num() > 0)
	{
		activeBattleGrid = Cast<ABattleGrid>(outBattleGrid[0]);
		activeBattleGrid->Init();
	}

	//Get all GridActors in level
	TArray<AActor*> outGridActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGridActor::StaticClass(), outGridActors);
	for (AActor* actor : outGridActors)
	{
		AGridActor* gridActor = Cast<AGridActor>(actor);
		gridActor->battleGrid = activeBattleGrid;
		gridActors.Add(gridActor);
	}

	//Remove all actors from level that have been previously destroyed and destroy their connected-ornamental actors
	UMainGameInstance* mainInstance = GameStatics::GetMainInstance(GetWorld());
	for (AActor* actorToDestroyOnLevelLoad : outGridActors)
	{
		for (FName& actorName : mainInstance->killedActors)
		{
			if (actorToDestroyOnLevelLoad->GetName() == actorName.ToString())
			{
				AGridActor* gridActor = Cast<AGridActor>(actorToDestroyOnLevelLoad);
				if (gridActor)
				{
					for (AActor* connectedActorToDestroy : gridActor->actorsToDestroyOnBreak)
					{
						connectedActorToDestroy->Destroy();
					}

					activeBattleGrid->UnhideNode(activeBattleGrid->GetNode(gridActor->xIndex, gridActor->yIndex));
				}

				actorToDestroyOnLevelLoad->Destroy();
			}
		}
	}
}
