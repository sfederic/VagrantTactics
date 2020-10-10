// Fill out your copyright notice in the Description page of Project Settings.

#include "VagrantTacticsGameModeBase.h"
#include "GridActor.h"
#include "Kismet/GameplayStatics.h"
#include "BattleGrid.h"
#include "Unit.h"

void AVagrantTacticsGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	//Get all GridActors in level
	TArray<AActor*> outGridActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGridActor::StaticClass(), outGridActors);
	for (AActor* actor : outGridActors)
	{
		AGridActor* gridActor = Cast<AGridActor>(actor);
		gridActors.Add(gridActor);
	}

	//Get BattleGrid
	TArray<AActor*> outBattleGrid;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABattleGrid::StaticClass(), outBattleGrid);
	check(outBattleGrid.Num() == 1);
	activeBattleGrid = Cast<ABattleGrid>(outBattleGrid[0]);

	for (AGridActor* gridActor : gridActors)
	{
		//activeBattleGrid->GetNode(gridActor->xIndex, gridActor->yIndex);
	}

	//Assign battle grid to all units
	TArray<AActor*> outUnits;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AUnit::StaticClass(), outUnits);
	for (AActor* actor : outUnits)
	{
		AUnit* unit = Cast<AUnit>(actor);
		unit->battleGrid = activeBattleGrid;
	}
}
