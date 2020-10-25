// Fill out your copyright notice in the Description page of Project Settings.

#include "VagrantTacticsGameModeBase.h"
#include "GridActor.h"
#include "Kismet/GameplayStatics.h"
#include "BattleGrid.h"
#include "Unit.h"
#include "PlayerUnit.h"

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
	activeBattleGrid = Cast<ABattleGrid>(outBattleGrid[0]);

	activeBattleGrid->Init();

	//Get all GridActors in level
	TArray<AActor*> outGridActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGridActor::StaticClass(), outGridActors);
	for (AActor* actor : outGridActors)
	{
		AGridActor* gridActor = Cast<AGridActor>(actor);
		gridActor->battleGrid = activeBattleGrid;
		gridActors.Add(gridActor);
	}
}
