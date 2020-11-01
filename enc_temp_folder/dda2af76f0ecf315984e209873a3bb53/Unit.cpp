
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
#include "SkillBase.h"
#include "TimerManager.h"
#include "UnitSkillWidget.h"

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
	if (particleFocusBeam)
	{
		particleFocusBeam->SetHiddenInGame(true);
	}

	//Setup skills
	for (int i = 0; i < skillClasses.Num(); i++)
	{
		skills.Add(NewObject<USkillBase>(this, skillClasses[i]));
	}
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

				//FRotator lookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), nextMoveLocation);
				//lookAtRotation.Yaw = 0.f;
				SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), nextMoveLocation));

				xIndex = pathNodes[movementPathNodeIndex]->xIndex;
				yIndex = pathNodes[movementPathNodeIndex]->yIndex;
				movementPathNodeIndex++;
			}
			else if (movementPathNodeIndex >= (pathNodes.Num())) //END OF MOVE
			{
				pathNodes.Empty();
				movementPathNodeIndex = 0;
				//bSetToMove = false;
				UE_LOG(LogTemp, Warning, TEXT("%s move finished."), *this->GetName());

				USkillBase* skillToUse = CycleThroughAttackChoices(actorToFocusOn);
				if (skillToUse)
				{
					//Sort of a testing case
					battleGrid->HideAllNodes();
					battleGrid->UnhideNodes(attackPathNodes);

					activeSkill = skillToUse;

					HighlightUnitOnSkillUse();
				}
				else
				{
					Attack();

					//TODO: This is only going to work niceley when one enemy is in the battle.
					//Otherwise you need to figure out how to mesh this with player's 'selectedUnit'
					ShowMovementPath();
				}

				//Deactive current standing node
				battleGrid->HideNode(battleGrid->GetNode(xIndex, yIndex));

				bTurnFinished = true;
			}
		}
	}

	SetActorLocation(FMath::VInterpConstantTo(GetActorLocation(), nextMoveLocation, DeltaTime, moveSpeed));
}

void AUnit::ShowMovementPath()
{
	//Handle if unit is charging skill
	if (bChargingSkill)
	{
		/*battleGrid->ResetAllNodeValues();
		battleGrid->HideAllNodes();

		auto skill = NewObject<USkillBase>(this, skillClasses[0]);
		ISkillInterface* skillInterface = Cast<ISkillInterface>(skill);
		skillInterface->UseSkill(0, 0, this, nullptr);

		battleGrid->UnhideNodes(attackPathNodes);

		return;*/
	}


	//Handle normal movement range
	int movementPoints = currentMovementPoints;

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

	//Activate previous standing node 
	if (pathNodes.Num() > 0)
	{
		battleGrid->UnhideNode(battleGrid->GetNode(xIndex, yIndex));
	}
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
					SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), targetNode->location));

					UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->PlayCameraShake(cameraShakeAttack);

					player->currentHealthPoints -= currentAttackPoints;

					UE_LOG(LogTemp, Warning, TEXT("%s attacked."), *GetName());
				}
			}
		}
	}
}

//Zoom in on unit, end turn after animation done and set widget to show skill name
void AUnit::HighlightUnitOnSkillUse()
{
	APlayerUnit* player = Cast<APlayerUnit>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	player->ZoomInOnTarget(Cast<AActor>(this));
	player->widgetUnitSkill->skillNameToDisplay = FText(activeSkill->skillName);
	player->widgetUnitSkill->AddToViewport();

	FTimerHandle handle;
	GetWorldTimerManager().SetTimer(handle, this, &AUnit::FinishDisplayingSkill, 2.0f, false);
}

//State AI function for checking whether a target is within the range of any unit spells/skills 
USkillBase* AUnit::CycleThroughAttackChoices(AActor* target)
{
	//Test case for skills
	for (int i = 0; i < skills.Num(); i++)
	{
		//auto skill = NewObject<USkillBase>(this, skillClasses[i]);
		ISkillInterface* skillInterface = Cast<ISkillInterface>(skills[i]);
		skillInterface->UseSkill(xIndex, yIndex, this, target);

		return skills[i];
	}

	return nullptr;
}

void AUnit::FinishDisplayingSkill()
{
	APlayerUnit* player = Cast<APlayerUnit>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	player->ResetCameraFocusAndFOV();
	player->widgetUnitSkill->RemoveFromViewport();
}
