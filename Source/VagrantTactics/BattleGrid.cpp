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

	//Populate grid and setup instances
	for (int x = 0; x < sizeX; x++)
	{
		rows.Add(GridRow());

		for (int y = 0; y < sizeY; y++)
		{
			FGridNode node = {};
			node.xIndex = x;
			node.yIndex = y;
			node.location = FVector((float)x * 100.f, (float)y * 100.f, 0.f);

			FTransform transform;
			transform.SetLocation(FVector((float)x * 100.f, (float)y * 100.f, -45.f));
			transform.SetScale3D(FVector(0.f));

			FHitResult hit;
			FCollisionQueryParams params;
			FVector endHit = transform.GetLocation() + FVector(0.f, 0.f, 45.f);
			if (GetWorld()->SweepSingleByChannel(hit, transform.GetLocation(), endHit, FQuat::Identity,
				ECC_WorldStatic, FCollisionShape::MakeBox(FVector(32.f))))
			{
				//Setting the scale for the instanced is the only way for now to disable their collision and visibility.
				transform.SetScale3D(FVector(0.f));
				node.bActive = false;
			}

			int32 instancedMeshIndex = instancedStaticMeshComponent->AddInstance(transform);

			//Add index to map
			nodeMap.Add(instancedMeshIndex, node);

			node.instancedMeshIndex = instancedMeshIndex;
			rows[x].columns.Add(node);
		}
	}
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
	if (currentX < (sizeX - 1))
	{
		FGridNode* node = &rows[currentX + 1].columns[currentY];
		if (!node->bClosed && node->bActive)
		{
			node->bClosed = true;
			node->parentNode = centerNode;
			outNodes.Add(node);
		}
	}

	//-X
	if (currentX > 0)
	{
		FGridNode* node = &rows[currentX - 1].columns[currentY];
		if (!node->bClosed && node->bActive)
		{
			node->bClosed = true;
			node->parentNode = centerNode;
			outNodes.Add(node);
		}
	}

	//+Y
	if (currentY < (sizeY - 1))
	{
		FGridNode* node = &rows[currentX].columns[currentY + 1];
		if (!node->bClosed && node->bActive)
		{
			node->bClosed = true;
			node->parentNode = centerNode;
			outNodes.Add(node);
		}
	}

	//-Y
	if (currentY > 0)
	{
		FGridNode* node = &rows[currentX].columns[currentY - 1];
		if (!node->bClosed && node->bActive)
		{
			node->bClosed = true;
			node->parentNode = centerNode;
			outNodes.Add(node);
		}
	}
}

void ABattleGrid::HideNodes(TArray<FGridNode*> nodesToHide)
{
	//Instance meshes need to have render dirty flag set to update transform.
	instancedStaticMeshComponent->MarkRenderStateDirty();

	for (FGridNode* node : nodesToHide)
	{
		FTransform transform;
		instancedStaticMeshComponent->GetInstanceTransform(node->instancedMeshIndex, transform);
		transform.SetScale3D(FVector(0.f));
		instancedStaticMeshComponent->UpdateInstanceTransform(node->instancedMeshIndex, transform);
	}
}

void ABattleGrid::UnhideNodes(TArray<FGridNode*> nodesToUnhide)
{
	instancedStaticMeshComponent->MarkRenderStateDirty();

	for (FGridNode* node : nodesToUnhide)
	{
		FTransform transform;
		instancedStaticMeshComponent->GetInstanceTransform(node->instancedMeshIndex, transform);
		transform.SetScale3D(FVector(0.95f));
		instancedStaticMeshComponent->UpdateInstanceTransform(node->instancedMeshIndex, transform);
	}
}

void ABattleGrid::ResetAllNodeValues()
{
	for (int x = 0; x < sizeX; x++)
	{
		for (int y = 0; y < sizeY; y++)
		{
			rows[x].columns[y].ResetValues();
		}
	}
}
