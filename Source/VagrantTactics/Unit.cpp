
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
#include "HealthbarWidget.h"
#include "SpeechComponent.h"
#include "Components/WidgetComponent.h"
#include "TimerManager.h"

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

	//Unit fear (shake unit)
	if (unitState == EUnitState::InFear)
	{
		FVector shakeLoc = GetActorLocation();
		float shakeLocZ = shakeLoc.Z;
		shakeLoc += FVector(FMath::RandRange(-5.0f, 5.0f));
		shakeLoc.Z = shakeLocZ;
		SetActorLocation(shakeLoc);
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
	if (pathNodes.Num() > 0 && bInBattle && !bWindingUpAttack)
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
			else if ((movementPathNodeIndex >= pathNodes.Num())) //END OF MOVE
			{
				pathNodes.Empty();
				movementPathNodeIndex = 0;
				UE_LOG(LogTemp, Warning, TEXT("%s move finished."), *this->GetName());

				battleGrid->HideNode(battleGrid->GetNode(xIndex, yIndex));

				//Initial melee attack
				if (Attack())
				{
					//ShowMovementPath();

				}
				else //If melee doesn't hit, move to skills
				{
					/*USkillBase* skillToUse = CycleThroughAttackChoices(actorToFocusOn);
					if (skillToUse)
					{
						battleGrid->HideAllNodes();
						battleGrid->UnhideNodes(attackPathNodes);

						FTimerHandle timerHandle;
						//TODO: going to have to add attack wind up time to skills eventually
						GetWorldTimerManager().SetTimer(timerHandle, this, &AUnit::WindUpSkill, attackWindUpTime, false);

						APlayerUnit* player = Cast<APlayerUnit>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
						player->ActivateGuardWindow(attackWindUpTime);

						activeSkill = skillToUse;

						HighlightUnitOnSkillUse();

						return;
					}*/

					bTurnFinished = true;
					bCurrentlyMoving = false;
				}
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
	case EUnitState::Stationary: nextNode = startingNode; break;
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
			meleeAttackNodeTarget = targetNode;
			for(FGridNode* node : neighbourNodes)
			{
				if (node->Equals(targetNode))
				{
					SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), targetNode->location));

					FTimerHandle timerHandle;
					GetWorldTimerManager().SetTimer(timerHandle, this, &AUnit::WindUpAttack, attackWindUpTime, false);
					
					player->currentCameraFOV = player->cameraFOVAttack;

					player->ActivateGuardWindow(attackWindUpTime);
						
					bWindingUpAttack = true;
					bSetToUseSkill = false;
					bChargingSkill = false;

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
		if (skillInterface)
		{
			skillInterface->ChargeSkill(xIndex, yIndex, this, target);
			bSetToUseSkill = true;
			return skills[i];
		}
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

void AUnit::ActivateForBattle()
{
	if (!bInBattle)
	{
		bInBattle = true;
		healthbarWidgetComponent->SetHiddenInGame(false);

		USpeechComponent* sc = FindComponentByClass<USpeechComponent>();
		if (sc)
		{
			sc->ShowDialogue(true);
		}
	}
}

//This function is to wait for unit attack animation to play out and give player guard chances
void AUnit::WindUpAttack()
{
	APlayerUnit* player = Cast<APlayerUnit>(actorToFocusOn);
	player->ResetCameraFocusAndFOV();

	//Check for doedge (if player is still in attack node paths)
	if (!meleeAttackNodeTarget->Equals(player->xIndex, player->yIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s attack missed."), *GetName());
	}
	else //Attack hits
	{
		UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->PlayCameraShake(cameraShakeAttack);

		if (!player->bIsGuarding)
		{
			player->currentHealthPoints -= currentAttackPoints;
			UE_LOG(LogTemp, Warning, TEXT("%s attacked."), *GetName());
		}
		else
		{
			player->bIsGuarding = false;
			UE_LOG(LogTemp, Warning, TEXT("%s attacked. Guarded."), *GetName());
		}
	}

	battleGrid->HideNode(battleGrid->GetNode(xIndex, yIndex));

	bWindingUpAttack = false;
	bCurrentlyMoving = false;
	bSetToUseSkill = false;
	bChargingSkill = false;
	bTurnFinished = true;
}

void AUnit::WindUpSkill()
{
	APlayerUnit* player = Cast<APlayerUnit>(actorToFocusOn);

	if (activeSkill)
	{
		for (FGridNode* attackNode : attackPathNodes)
		{
			if (attackNode->Equals(player->xIndex, player->yIndex))
			{
				UE_LOG(LogTemp, Warning, TEXT("%s skill %s hit."), *GetName(), *activeSkill->GetName());

				UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->PlayCameraShake(cameraShakeAttack);

				if(!player->bIsGuarding)
				{
					player->currentHealthPoints -= currentAttackPoints;
				}
				else
				{
					player->bIsGuarding = false;
				}

				break;
			}
		}
	}

	battleGrid->HideNode(battleGrid->GetNode(xIndex, yIndex));
	battleGrid->HideNodes(attackPathNodes);

	ShowMovementPath();

	bWindingUpAttack = false;
	bCurrentlyMoving = false;
	bSetToUseSkill = false;
	bChargingSkill = false;
	bTurnFinished = true;
}

//Effect to activate when unit hits max stress
void AUnit::ActivateStress()
{
	UE_LOG(LogTemp, Warning, TEXT("%s is under stress."), *GetName());
	USpeechComponent* sc = FindComponentByClass<USpeechComponent>();
	sc->ShowStressDialogue();

	bUnderStress = true;
	currentMovementPoints = movementPointsUnderStress;
	currentAttackPoints = attackPointsUnderStress;
	unitState = stateUnderStress;
}

//Call this over Destroy(). Handles battlegrid numbers as well
void AUnit::RemoveFromMap()
{

	//battleGrid->numOfUnitsAlive--;
	Destroy();
}
