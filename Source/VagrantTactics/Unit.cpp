// Fill out your copyright notice in the Description page of Project Settings.

#include "Unit.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerUnit.h"
#include "LevelGridValues.h"
#include "BattleGrid.h"
#include "Kismet/GameplayStatics.h"
#include "HealthbarWidget.h"
#include "Components/WidgetComponent.h"

AUnit::AUnit()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AUnit::BeginPlay()
{
	Super::BeginPlay();
	
	currentMovementPoints = maxMovementPoints;

	nextMoveLocation = GetActorLocation();

	bIsDestructible = true;

	//Setup health bar
	healthbarWidget = Cast<UHealthbarWidget>(FindComponentByClass<UWidgetComponent>()->GetUserWidgetObject());
	healthbarWidget->attachedUnit = this;
}

void AUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Movement path
	if (GetActorLocation().Equals(nextMoveLocation) && bSetToMove)
	{
		if (pathNodes.Num() > 0)
		{
			if (movementPathNodeIndex < pathNodes.Num())
			{
				nextMoveLocation = pathNodes[movementPathNodeIndex]->location;
				xIndex = pathNodes[movementPathNodeIndex]->xIndex;
				yIndex = pathNodes[movementPathNodeIndex]->yIndex;
				movementPathNodeIndex++;
			}

			if (movementPathNodeIndex > (pathNodes.Num() - 1))
			{
				pathNodes.Empty();
				movementPathNodeIndex = 0;
				bSetToMove = false;
				bTurnFinished = true;
			}
		}
	}

	SetActorLocation(FMath::VInterpConstantTo(GetActorLocation(), nextMoveLocation, DeltaTime, moveSpeed));
}

void AUnit::ShowMovementPath(int movementPoints)
{
	battleGrid->ResetAllNodeValues();

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

	battleGrid->UnhideNodes(previewNodes);

	for (FGridNode* node : previewNodes)
	{
		movementPathNodes.Add(node);
	}
}

void AUnit::MoveTo(FGridNode* destinationNode)
{
	bSetToMove = true;

	//Set player camera focus
	APlayerUnit* player = Cast<APlayerUnit>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	player->selectedUnit = this;

	FGridNode* startingNode = battleGrid->GetNode(xIndex, yIndex);

	//Assign all costs
	for (int i = 0; i < movementPathNodes.Num(); i++)
	{
		movementPathNodes[i]->gCost = FVector::Distance(startingNode->location, movementPathNodes[i]->location);
		movementPathNodes[i]->hCost = FVector::Distance(destinationNode->location, movementPathNodes[i]->location);
	}

	//Find lowest distance to end
	int lowestHCostIndex = 0;
	float lowestHCost = TNumericLimits<float>::Max();
	for (int i = 0; i < movementPathNodes.Num(); i++)
	{
		if (movementPathNodes[i]->hCost < lowestHCost)
		{
			lowestHCost = movementPathNodes[i]->hCost;
			lowestHCostIndex = i;
		}
	}

	FGridNode* nextNode = movementPathNodes[lowestHCostIndex];

	while (nextNode != startingNode)
	{
		nextNode = nextNode->parentNode;
		pathNodes.Add(nextNode);
	}

	Algo::Reverse(pathNodes);
	movementPathNodes.Empty();

	return;
}
