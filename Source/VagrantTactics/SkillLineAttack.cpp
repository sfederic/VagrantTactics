// Fill out your copyright notice in the Description page of Project Settings.

#include "SkillLineAttack.h"
#include "Unit.h"
#include "PlayerUnit.h"
#include "GridNode.h"
#include "BattleGrid.h"

void USkillLineAttack::UseSkill(int x, int y, AUnit* owner, AGridActor* target)
{
	if (owner)
	{
		owner->bChargingSkill = true;

		if (owner->actorToFocusOn->IsA<APlayerUnit>())
		{
			APlayerUnit* player = Cast<APlayerUnit>(owner->actorToFocusOn);
			if (player)
			{
				TArray<FGridNode*> attackPathNodes;

				ABattleGrid* battleGrid = owner->battleGrid;

				const int attackRange = 3;

				//if (player->xIndex > owner->xIndex && player->yIndex < owner->yIndex)
				{
					for (int x = owner->xIndex; x > 0; x--)
					{
						if (x > 0 && x < battleGrid->sizeX)
						{
							attackPathNodes.Add(battleGrid->GetNode(x, owner->yIndex));
						}
						else
						{
							break;
						}
					}
				}

				owner->attackPathNodes.Empty();
				owner->attackPathNodes = attackPathNodes;
			}
		}
	}
}
