// Fill out your copyright notice in the Description page of Project Settings.

#include "SkillLineAttack.h"
#include "Unit.h"
#include "PlayerUnit.h"
#include "GridNode.h"
#include "BattleGrid.h"

//Skill forms a line in front of unit. Think SAM skills in Tactics.
void USkillLineAttack::UseSkill(int x, int y, AUnit* owner, AActor* target)
{
	//Check if attack hits
	{
		if (target->IsA<APlayerUnit>())
		{
			APlayerUnit* player = Cast<APlayerUnit>(target);
			FGridNode* targetNode = owner->battleGrid->GetNode(player->xIndex, player->yIndex);
			if (owner->attackPathNodes.Contains(targetNode))
			{
				player->currentHealthPoints -= attackDamage;
			}
		}
		else if (target->IsA<AGridActor>())
		{
			AGridActor* gridActor = Cast<AGridActor>(target);
			FGridNode* targetNode = owner->battleGrid->GetNode(gridActor->xIndex, gridActor->yIndex);
			if (owner->attackPathNodes.Contains(targetNode))
			{
				gridActor->currentHealth -= attackDamage;
			}
		}
	}
}

void USkillLineAttack::ChargeSkill(int x, int y, AUnit* owner, AActor* target)
{
	TArray<FGridNode*> attackPathNodes;

	if (bIsChargingSkill)
	{
		owner->bChargingSkill = true;
	}

	if (owner)
	{
		if (owner->actorToFocusOn == nullptr)
		{
			return;
		}

		ABattleGrid* battleGrid = owner->battleGrid;

		//Get line based on forward direction
		FVector forwardVec = owner->GetActorForwardVector();

		if (forwardVec.Equals(FVector(1.f, 0.f, 0.f)))
		{
			for (int x = owner->xIndex; x < (owner->xIndex + attackRange); x++)
			{
				if (x < battleGrid->sizeX)
				{
					attackPathNodes.Add(battleGrid->GetNode(x, owner->yIndex));
				}
			}
		}
		else if (forwardVec.Equals(FVector(-1.f, 0.f, 0.f)))
		{
			for (int x = owner->xIndex; x > (owner->xIndex - attackRange); x--)
			{
				if (x > 0)
				{
					attackPathNodes.Add(battleGrid->GetNode(x, owner->yIndex));
				}
			}
		}
		else if (forwardVec.Equals(FVector(0.f, 1.f, 0.f)))
		{
			for (int y = owner->yIndex; y < (owner->yIndex + attackRange); y++)
			{
				if (y < battleGrid->sizeY)
				{
					attackPathNodes.Add(battleGrid->GetNode(owner->xIndex, y));
				}
			}
		}
		else if (forwardVec.Equals(FVector(0.f, -1.f, 0.f)))
		{
			for (int y = owner->yIndex; y > (owner->yIndex - attackRange); y--)
			{
				if (y > 0)
				{
					attackPathNodes.Add(battleGrid->GetNode(owner->xIndex, y));
				}
			}
		}
	}

	owner->attackPathNodes.Empty();
	owner->attackPathNodes = attackPathNodes;
}
