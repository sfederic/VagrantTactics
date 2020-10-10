// Fill out your copyright notice in the Description page of Project Settings.

#include "Unit.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerUnit.h"
#include "LevelGridValues.h"
#include "BattleGrid.h"

AUnit::AUnit()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AUnit::BeginPlay()
{
	Super::BeginPlay();
	
	currentHealth = maxHealth;
	currentMovementPoints = maxMovementPoints;

	nextMoveLocation = GetActorLocation();

	xIndex = FMath::RoundToInt(GetActorLocation().X / LevelGridValues::gridUnitDistance);
	yIndex = FMath::RoundToInt(GetActorLocation().Y / LevelGridValues::gridUnitDistance);
}

void AUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*if (GetActorLocation().Equals(nextMoveLocation))
	{
		if (movementPathNodes.Num() > 0)
		{
			if (movementPathNodeIndex < movementPathNodes.Num())
			{
				nextMoveLocation = movementPathNodes[movementPathNodeIndex].location;
				movementPathNodeIndex++;
			}

			if (movementPathNodeIndex > (movementPathNodes.Num() - 1))
			{
				movementPathNodes.Empty();
			}
		}
	}

	SetActorLocation(FMath::VInterpConstantTo(GetActorLocation(), nextMoveLocation, DeltaTime, moveSpeed));*/

	if (currentHealth <= 0)
	{
		APlayerUnit* player = Cast<APlayerUnit>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		player->ResetActionPointsToMax();
		Destroy();
	}
}

void AUnit::ShowMovementPath(int movementPoints)
{
	battleGrid->ResetAllNodes();

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
}

void AUnit::MoveTo(FGridNode* destinationNode)
{
	battleGrid->ResetAllNodes();

	FGridNode* startingNode = &battleGrid->rows[xIndex].columns[yIndex];
	startingNode->gCost = FVector::Dist(startingNode->location, startingNode->location);
	startingNode->hCost = FVector::Dist(startingNode->location, destinationNode->location);
	startingNode->parentNode = nullptr;

	FGridNode* currentNode = startingNode;

	TArray<FGridNode*> openNodes;
	openNodes.Add(currentNode);

	TArray<FGridNode*> closedNodes;

	while (openNodes.Num() > 0)
	{
		//Find node with lowest F-cost in array.
		float lowestFCost = TNumericLimits<float>::Max();
		int currentIndex = 0;
		for (int i = 0; i < openNodes.Num(); i++)
		{
			float fCost = openNodes[i]->GetFCost();
			if (fCost <= lowestFCost)
			{
				lowestFCost = fCost;
				currentIndex = i;
			}
		}

		currentNode = openNodes[currentIndex];
		currentNode->bClosed = true;
		closedNodes.Add(currentNode);
		openNodes.RemoveAt(currentIndex);

		//Check if currentNode is destinationNode
		if (currentNode == destinationNode)
		{
			goto EndLoop;
		}

		//Get each cross neighbour of the current node
		int currentX = currentNode->xIndex;
		int currentY = currentNode->yIndex;

		//+X
		if (currentX < (battleGrid->sizeX - 1))
		{
			FGridNode* node = &battleGrid->rows[currentX + 1].columns[currentY];
			if (node->bActive && !node->bClosed)
			{
				node->gCost = FVector::Dist(node->location, startingNode->location);
				node->hCost = FVector::Dist(node->location, destinationNode->location);
				node->parentNode = currentNode;

				openNodes.Add(node);
			}
		}

		//-X
		if (currentX != 0)
		{
			FGridNode* node = &battleGrid->rows[currentX - 1].columns[currentY];
			if (node->bActive && !node->bClosed)
			{
				node->gCost = FVector::Dist(node->location, startingNode->location);
				node->hCost = FVector::Dist(node->location, destinationNode->location);
				node->parentNode = currentNode;

				openNodes.Add(node);
			}
		}

		//+Y
		if (currentY < (battleGrid->sizeY - 1))
		{
			FGridNode* node = &battleGrid->rows[currentX].columns[currentY + 1];
			if (node->bActive && !node->bClosed)
			{
				node->gCost = FVector::Dist(node->location, startingNode->location);
				node->hCost = FVector::Dist(node->location, destinationNode->location);
				node->parentNode = currentNode;

				openNodes.Add(node);
			}
		}

		//-Y
		if (currentY != 0)
		{
			FGridNode* node = &battleGrid->rows[currentX].columns[currentY - 1];
			if (node->bActive && !node->bClosed)
			{
				node->gCost = FVector::Dist(node->location, startingNode->location);
				node->hCost = FVector::Dist(node->location, destinationNode->location);
				node->parentNode = currentNode;

				openNodes.Add(node);
			}
		}
	}
	
	EndLoop:

	//Trace path from destination node to starting node position through parents
	movementPathNodes.Add(*currentNode);

	while (currentNode->parentNode != nullptr)
	{
		currentNode = currentNode->parentNode;
		movementPathNodes.Add(*currentNode);
	}

	Algo::Reverse(movementPathNodes);

	openNodes.Empty();
	closedNodes.Empty();
}
