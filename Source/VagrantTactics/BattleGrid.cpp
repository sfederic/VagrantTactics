// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleGrid.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "GameplayTags.h"

ABattleGrid::ABattleGrid()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ABattleGrid::BeginPlay()
{
	Super::BeginPlay();
	
	instancedStaticMeshComponent = FindComponentByClass<UInstancedStaticMeshComponent>();

	for (uint32 x = 0; x < sizeX; x++)
	{
		rows.Add(GridRow());

		for (uint32 y = 0; y < sizeY; y++)
		{
			FTransform transform;
			transform.SetLocation(FVector((float)x * 100.f, (float)y * 100.f, -45.f));
			transform.SetScale3D(FVector(0.95f));

			FHitResult hit;
			FCollisionQueryParams params;
			FVector endHit = transform.GetLocation() + FVector(0.f, 0.f, 45.f);
			if (GetWorld()->SweepSingleByChannel(hit, transform.GetLocation(), endHit, FQuat::Identity,
				ECC_WorldStatic, FCollisionShape::MakeBox(FVector(32.f))))
			{
				if (!hit.GetActor()->Tags.Contains(GameplayTags::Player))
				{
					//Setting the scale for the instanced is the only way for now to disable their collision and visibility.
					transform.SetScale3D(FVector(0.f));
				}
			}

			int32 instancedMeshIndex = instancedStaticMeshComponent->AddInstance(transform);
			//instancedStaticMeshComponent->UpdateInstanceTransform(0, FTransform(FVector(0.f)));
			rows[x].columns.Add(FGridNode(x, y, instancedMeshIndex));
		}
	}

	//instancedStaticMeshComponent->SetHiddenInGame(true);
}

void ABattleGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FGridNode* ABattleGrid::GetNode(int x, int y)
{
	check(x < sizeX);
	check(y < sizeY);
	return &rows[x].columns[y];
}

void ABattleGrid::ActivateBattle()
{
	bBattleActive = !bBattleActive;

	if (bBattleActive)
	{
		instancedStaticMeshComponent->SetHiddenInGame(false);
	}
	else if (!bBattleActive)
	{
		instancedStaticMeshComponent->SetHiddenInGame(true);
	}
}

void ABattleGrid::GetNeighbouringNodes(FGridNode* centerNode, TArray<FGridNode*>& outNodes)
{
	int currentX = centerNode->xIndex;
	int currentY = centerNode->yIndex;

	//+X
	if (currentX < sizeX)
	{
		outNodes.Add(&rows[currentX + 1].columns[currentY]);
	}
}
