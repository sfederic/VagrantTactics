
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
#include "GameplayTags.h"

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

	//Particle beam Focus 
	if (actorToFocusOn && !particleFocusBeam->bHiddenInGame)
	{
		particleFocusBeam->SetBeamSourcePoint(0, GetActorLocation(), 0);
		particleFocusBeam->SetBeamEndPoint(0, actorToFocusOn->GetActorLocation());
	}


	//Get all actors to focus on that match with unit tags to focus on
	for (int focusTagIndex = 0; focusTagIndex < focusTags.Num(); focusTagIndex++)
	{
		TArray<AActor*> focusActors;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), focusTags[focusTagIndex], focusActors);
		for (AActor* actor : focusActors)
		{
			actorToFocusOn = actor;
			unitState = EUnitState::Flee;
		}

		if (focusActors.Num() == 0)
		{
			actorToFocusOn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		}
	}

	//Movement path logic
	if (pathNodes.Num() > 0 && bInBattle)
	{
		bCurrentlyMoving = true;

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
			else if (movementPathNodeIndex >= pathNodes.Num()) //END OF MOVE
			{
				pathNodes.Empty();
				movementPathNodeIndex = 0;
				UE_LOG(LogTemp, Warning, TEXT("%s move finished."), *this->GetName());

				//Initial melee attack
				if (Attack())
				{
					ShowMovementPath();
				}
				else //If melee doessn't hit, move to skills
				{
					USkillBase* skillToUse = CycleThroughAttackChoices(actorToFocusOn);
					if (skillToUse)
					{
						battleGrid->HideAllNodes();
						battleGrid->UnhideNodes(attackPathNodes);

						activeSkill = skillToUse;

						HighlightUnitOnSkillUse();
					}
				}

				//Deactive current standing node
				battleGrid->HideNode(battleGrid->GetNode(xIndex, yIndex));

				bCurrentlyMoving = false;
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
		battleGrid->ResetAllNodeValues();
		battleGrid->HideAllNodes();
		battleGrid->UnhideNodes(attackPathNodes);

		return;
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
	if (movementPathNodes.Num() == 0)
	{
		return;
	}

	if (bChargingSkill)
	{
		return;
	}

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

	//Find lowest distance to 'end' based on UnitState
	int highestHCostIndex = 0;
	int lowestHCostIndex = 0;

	if (unitState == EUnitState::Chase)
	{
		float lowestHCost = TNumericLimits<float>::Max();
		for (int i = 0; i < movementPathNodes.Num(); i++)
		{
			if (movementPathNodes[i]->hCost < lowestHCost)
			{
				lowestHCost = movementPathNodes[i]->hCost;
				lowestHCostIndex = i;
			}
		}
	}
	else if (unitState == EUnitState::Flee)
	{
		float highestHCost = -1.f;
		for (int i = 0; i < movementPathNodes.Num(); i++)
		{
			if (movementPathNodes[i]->hCost >= highestHCost)
			{
				highestHCost = movementPathNodes[i]->hCost;
				highestHCostIndex = i;
			}
		}
	}

	FGridNode* nextNode = nullptr;
	switch (unitState)
	{
	case EUnitState::Chase:	nextNode = movementPathNodes[lowestHCostIndex]; break;
	case EUnitState::Flee: nextNode = movementPathNodes[highestHCostIndex]; break;
	case EUnitState::Wander: 
		int randomNodeIndex = FMath::RandRange(0, movementPathNodes.Num() - 1);
		nextNode = movementPathNodes[randomNodeIndex];
		break;
	}

	while (nextNode != startingNode)
	{
		if (nextNode->parentNode)
		{
			nextNode = nextNode->parentNode;
			pathNodes.Add(nextNode);
		}
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

FGridNode* AUnit::FindTargetFocusNode()
{
	//TODO: keep an eye on this. Non-standard way getting around xIndex/yIndex for now
	FVector loc = actorToFocusOn->GetActorLocation();
	int x = FMath::RoundToInt(loc.X / 100.f);
	int y = FMath::RoundToInt(loc.Y / 100.f);

	return battleGrid->GetNode(x, y);
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
//The bool return is just to work with skills and spells in tick (skill goes off if attack doesn't land)
bool AUnit::Attack()
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

					return true;
				}
			}
		}
	}

	return false;
}

//Zoom in on unit, end turn after animation done and set widget to show skill name
void AUnit::HighlightUnitOnSkillUse()
{
	APlayerUnit* player = Cast<APlayerUnit>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	player->ZoomInOnTarget(Cast<AActor>(this));
	player->widgetUnitSkill->skillNameToDisplay = activeSkill->skillName;
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
		ISkillInterface* skillInterface = Cast<ISkillInterface>(skills[i]);
		skillInterface->ChargeSkill(xIndex, yIndex, this, target);

		bSetToUseSkill = true;

		return skills[i];
	}

	return nullptr;
}

void AUnit::FinishDisplayingSkill()
{
	APlayerUnit* player = Cast<APlayerUnit>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	player->ResetCameraFocusAndFOV();
	player->widgetUnitSkill->RemoveFromViewport();

	bTurnFinished = true;
}

void AUnit::UseSkill()
{
	if (bSetToUseSkill && (attackPathNodes.Num() > 0))
	{
		ISkillInterface* skillInterface = Cast<ISkillInterface>(activeSkill);
		skillInterface->UseSkill(xIndex, yIndex, this, actorToFocusOn);

		UE_LOG(LogTemp, Warning, TEXT("%s skill used %s"), *GetName(), *activeSkill->skillName.ToString());

		attackPathNodes.Empty();

		bTurnFinished = true;
		bSetToUseSkill = false;
		bChargingSkill = false;
	}
}
