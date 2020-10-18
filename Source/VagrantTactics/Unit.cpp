// Fill out your copyright notice in the Description page of Project Settings.

#include "Unit.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "PlayerUnit.h"
#include "LevelGridValues.h"
#include "BattleGrid.h"
#include "Particles/ParticleSystemComponent.h"
#include "Camera/CameraShake.h"
#include "GameStatics.h"

AUnit::AUnit()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AUnit::BeginPlay()
{
	Super::BeginPlay();
	
	currentMovementPoints = maxMovementPoints;
	currentAttackDistancePoints = maxAttackDistancePoints;
	currentAttackPoints = maxAttackPoints;

	nextMoveLocation = GetActorLocation();

	bIsDestructible = true;

	//Setup focus particle
	particleFocusBeam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), particleTemplateFocusBeam, FTransform(GetActorLocation()));
	particleFocusBeam->SetHiddenInGame(true);
}

void AUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (actorToFocusOn && !particleFocusBeam->bHiddenInGame)
	{
		particleFocusBeam->SetBeamSourcePoint(0, GetActorLocation(), 0);
		particleFocusBeam->SetBeamEndPoint(0, actorToFocusOn->GetActorLocation());
	}

	//Movement path
	if (pathNodes.Num() > 0 && bInBattle)
	{
		if ((GetActorLocation().Equals(nextMoveLocation)) && (bSetToMove) && (!bTurnFinished))
		{
			if (movementPathNodeIndex < pathNodes.Num())
			{
				nextMoveLocation = pathNodes[movementPathNodeIndex]->location;

				SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), nextMoveLocation));

				xIndex = pathNodes[movementPathNodeIndex]->xIndex;
				yIndex = pathNodes[movementPathNodeIndex]->yIndex;
				movementPathNodeIndex++;
			}
			else if (movementPathNodeIndex >= (pathNodes.Num()))
			{
				pathNodes.Empty();
				movementPathNodeIndex = 0;
				//bSetToMove = false;
				UE_LOG(LogTemp, Warning, TEXT("move finished"));

				Attack();

				bTurnFinished = true;
			}
		}
	}

	SetActorLocation(FMath::VInterpConstantTo(GetActorLocation(), nextMoveLocation, DeltaTime, moveSpeed));
}

void AUnit::ShowMovementPath(int movementPoints)
{
	battleGrid->ResetAllNodeValues();
	battleGrid->HideAllNodes();

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

void AUnit::HideMovementPath()
{
	battleGrid->HideAllNodes();
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
}

FGridNode* AUnit::FindPlayerNode()
{
	APlayerUnit* player = Cast<APlayerUnit>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	return battleGrid->GetNode(player->xIndex, player->yIndex);
}

void AUnit::FindPointOfInterest()
{

}

void AUnit::ShowUnitFocus()
{
	//Testing player focus
	APawn* player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	actorToFocusOn = player;

	particleFocusBeam->SetHiddenInGame(false);
	particleFocusBeam->SetBeamSourcePoint(0, GetActorLocation(), 0);
	particleFocusBeam->SetBeamEndPoint(0, actorToFocusOn->GetActorLocation());
}

void AUnit::HideUnitFocus()
{
	particleFocusBeam->SetHiddenInGame(true);
	particleFocusBeam->SetBeamSourcePoint(0, GetActorLocation(), 0);
	particleFocusBeam->SetBeamEndPoint(0, GetActorLocation());
}

//function covers Melee attack (adjacent grid node to unit)
void AUnit::Attack()
{
	if (actorToFocusOn)
	{
		APlayerUnit* player = Cast<APlayerUnit>(actorToFocusOn);
		if (player)
		{
			battleGrid->ResetAllNodeValues();

			FGridNode* currentNode = battleGrid->GetNode(xIndex, yIndex);
			TArray<FGridNode*> neighbourNodes;
			battleGrid->GetNeighbouringNodes(currentNode, neighbourNodes);

			FGridNode* targetNode = battleGrid->GetNode(player->xIndex, player->yIndex);
			for(FGridNode* node : neighbourNodes)
			{
				if (node->Equals(targetNode))
				{
					UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), targetNode->location);

					UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->PlayCameraShake(cameraShakeAttack);

					player->currentHealthPoints -= currentAttackPoints;

					UE_LOG(LogTemp, Warning, TEXT("%s attacked."), *GetName());
				}
			}
		}
	}
}
