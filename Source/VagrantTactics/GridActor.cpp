// Fill out your copyright notice in the Description page of Project Settings.

#include "GridActor.h"
#include "LevelGridValues.h"
#include "BattleGrid.h"

AGridActor::AGridActor()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AGridActor::BeginPlay()
{
	Super::BeginPlay();
	
	currentHealth = maxHealth;

	xIndex = FMath::RoundToInt(GetActorLocation().X / LevelGridValues::gridUnitDistance);
	yIndex = FMath::RoundToInt(GetActorLocation().Y / LevelGridValues::gridUnitDistance);
}

void AGridActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsDestructible)
	{
		if (currentHealth <= 0)
		{
			battleGrid->UnhideNodes(connectedNodes);

			Destroy();
		}
	}
}

