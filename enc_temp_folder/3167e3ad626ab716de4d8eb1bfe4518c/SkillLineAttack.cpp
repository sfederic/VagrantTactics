// Fill out your copyright notice in the Description page of Project Settings.

#include "SkillLineAttack.h"
#include "Unit.h"
#include "PlayerUnit.h"
#include "GridNode.h"
#include "BattleGrid.h"

//Skill forms a line in front of unit. Think SAM attacks in Tactics.
void USkillLineAttack::UseSkill(int x, int y, AUnit* owner, AActor* target)
{
	TArray<FGridNode*> attackPathNodes;

	if (owner)
	{
		owner->bChargingSkill = true;

		if (owner->actorToFocusOn == nullptr)
		{
			return;
		}


		ABattleGrid* battleGrid = owner->battleGrid;

		const int attackRange = 3;

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
