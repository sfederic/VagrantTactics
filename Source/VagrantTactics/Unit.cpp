// Fill out your copyright notice in the Description page of Project Settings.

#include "Unit.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerUnit.h"
#include "LevelGridValues.h"
#include "GridNode.h"
#include "BattleGrid.h"

AUnit::AUnit()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AUnit::BeginPlay()
{
	Super::BeginPlay();
	
	currentHealth = maxHealth;

	xIndex = FMath::RoundToInt(GetActorLocation().X / LevelGridValues::gridUnitDistance);
	yIndex = FMath::RoundToInt(GetActorLocation().Y / LevelGridValues::gridUnitDistance);
}

void AUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (currentHealth <= 0)
	{
		APlayerUnit* player = Cast<APlayerUnit>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		player->ResetActionPointsToMax();
		Destroy();
	}
}

void AUnit::ShowMovementPath(int movementPoints)
{
	FGridNode* startingNode = battleGrid->GetNode(xIndex, yIndex);

	TArray<FGridNode*> previewNodes;
	TArray<FGridNode*> closedPreviewNodes;

	battleGrid->GetNeighbouringNodes(startingNode, previewNodes);

	for (int moveIndex = 0; moveIndex < movementPoints; moveIndex++)
	{
		for (int previewIndex = 0; previewIndex < previewNodes.Num(); previewIndex++)
		{
			battleGrid->GetNeighbouringNodes(previewNodes[previewIndex], closedPreviewNodes);
		}

		previewNodes.Append(closedPreviewNodes);
		closedPreviewNodes.Empty();
	}

	for (FGridNode* node : previewNodes)
	{
		FTransform transform;
		battleGrid->instancedStaticMeshComponent->GetInstanceTransform(node->instancedMeshIndex, transform);
		transform.SetScale3D(FVector(0.f));
		battleGrid->instancedStaticMeshComponent->UpdateInstanceTransform(node->instancedMeshIndex, transform);
	}

	return;
}
