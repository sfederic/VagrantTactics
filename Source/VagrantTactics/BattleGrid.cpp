// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleGrid.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "GameplayTags.h"
#include "GridActor.h"
#include "LevelGridValues.h"
#include "Kismet/GameplayStatics.h"
#include "Unit.h"
#include "PlayerUnit.h"
#include "Components/WidgetComponent.h"

ABattleGrid::ABattleGrid()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ABattleGrid::BeginPlay()
{
	Super::BeginPlay();
	
	//Get all units and sort
	TArray<AActor*> outUnits;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AUnit::StaticClass(), outUnits);
	allUnits.Reserve(outUnits.Num());
	for (AActor* actor : outUnits)
	{
		AUnit* unit = Cast<AUnit>(actor);
		allUnits.Add(unit);
	}

	SortUnitsByTurnSpeed();
}

void ABattleGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Iterate through enemy turn list based on highest speed first
	if (bEnemyTurn)
	{
		if (allUnits.Num() > 0)
		{
			if ((!allUnits[activeUnitIndex]->bSetToMove) /*&& (allUnits[activeUnitIndex]->pathNodes.Num() == 0)*/)
			{
				if (allUnits[activeUnitIndex]->pathNodes.Num() < 1)
				{
					UE_LOG(LogTemp, Warning, TEXT("move to hit again"));
					allUnits[activeUnitIndex]->ShowMovementPath(allUnits[activeUnitIndex]->currentMovementPoints);
					allUnits[activeUnitIndex]->MoveTo(allUnits[activeUnitIndex]->FindPlayerNode());
				}
			}

			if (allUnits[activeUnitIndex]->bTurnFinished)
			{
				if (activeUnitIndex < (allUnits.Num() - 1))
				{
					activeUnitIndex++;
				}
				else
				{
					//End loop 
					activeUnitIndex = 0;
					allUnits[activeUnitIndex]->bSetToMove = false;
					allUnits[activeUnitIndex]->bTurnFinished = true;
					ChangeTurn();
				}
			}
		}
	}
}

FGridNode* ABattleGrid::GetNode(int x, int y)
{
	check(x < sizeX);
	check(y < sizeY);
	return &rows[x].columns[y];
}

void ABattleGrid::Init()
{
	instancedStaticMeshComponent = FindComponentByClass<UInstancedStaticMeshComponent>();
	check(instancedStaticMeshComponent);
	
	//Populate grid and setup instances
	for (int x = 0; x < sizeX; x++)
	{
		rows.Add(GridRow());

		for (int y = 0; y < sizeY; y++)
		{
			FGridNode node = {};
			node.xIndex = x;
			node.yIndex = y;
			node.location = FVector((float)x * LevelGridValues::gridUnitDistance, (float)y * LevelGridValues::gridUnitDistance, 0.f);
			node.bActive = true;

			FTransform transform;
			transform.SetLocation(FVector((float)x * LevelGridValues::gridUnitDistance, (float)y * LevelGridValues::gridUnitDistance, -LevelGridValues::nodeHeightOffset));
			transform.SetScale3D(nodeVisibleScale);

			FHitResult hit;
			FCollisionQueryParams params;
			FVector endHit = transform.GetLocation() + FVector(0.f, 0.f, 1000.f);
			if (GetWorld()->SweepSingleByChannel(hit, transform.GetLocation(), endHit, FQuat::Identity,
				ECC_WorldStatic, FCollisionShape::MakeBox(FVector(32.f))))
			{
				if (!hit.GetActor()->Tags.Contains(GameplayTags::Player))
				{
					//Setting the scale for the instanced is the only way for now to disable their collision and visibility.
					transform.SetScale3D(nodeHiddenScale);
					node.bActive = false; 
				}

				//Deal with platforms
				if (hit.GetActor()->Tags.Contains(GameplayTags::Platform))
				{
					FHitResult platformHit;
					FVector startHit = transform.GetLocation() + FVector(0.f, 0.f, 1000.f);
					if (GetWorld()->LineTraceSingleByChannel(platformHit, startHit, transform.GetLocation(), ECC_WorldStatic, params))
					{
						transform.SetLocation(platformHit.ImpactPoint + FVector(0.f, 0.f, 5.f));
						transform.SetScale3D(nodeVisibleScale);
						node.location = transform.GetLocation() + FVector(0.f, 0.f, LevelGridValues::nodeHeightOffset);
						node.bActive = true;
					}
				}
			}

			int32 instancedMeshIndex = instancedStaticMeshComponent->AddInstance(transform);

			node.instancedMeshIndex = instancedMeshIndex;
			rows[x].columns.Add(node);

			//Add node to map through instance mesh index
			nodeMap.Add(instancedMeshIndex, rows[x].columns[y]);

			if (hit.GetActor())
			{
				if (!hit.GetActor()->Tags.Contains(GameplayTags::Player))
				{
					AGridActor* hitGridActor = Cast<AGridActor>(hit.GetActor());
					if (hitGridActor)
					{
						hitGridActor->connectedNodeIndices.Add(node.instancedMeshIndex);
					}
				}
			}
		}
	}
}

void ABattleGrid::ActivateBattle()
{
	bBattleActive = !bBattleActive;

	if (bBattleActive)
	{
		instancedStaticMeshComponent->SetHiddenInGame(false);

		//Show all grid actor health bars on battle start
		TArray<AActor*> outGridActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGridActor::StaticClass(), outGridActors);
		for (AActor* actor : outGridActors)
		{
			AGridActor* gridActor = Cast<AGridActor>(actor);
			gridActor->healthbarWidgetComponent->SetHiddenInGame(false);
		}
	}
	else if (!bBattleActive)
	{
		instancedStaticMeshComponent->SetHiddenInGame(true);

		//Hide all grid actor health bars on battle end
		TArray<AActor*> outGridActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGridActor::StaticClass(), outGridActors);
		for (AActor* actor : outGridActors)
		{
			AGridActor* gridActor = Cast<AGridActor>(actor);
			gridActor->healthbarWidgetComponent->SetHiddenInGame(true);
		}
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

void ABattleGrid::HideNodes(TArray<FGridNode*>& nodesToHide)
{
	//Instance meshes need to have render dirty flag set to update transform.
	instancedStaticMeshComponent->MarkRenderStateDirty();

	for (FGridNode* node : nodesToHide)
	{
		node->bActive = false;

		FTransform transform;
		instancedStaticMeshComponent->GetInstanceTransform(node->instancedMeshIndex, transform);
		transform.SetScale3D(nodeHiddenScale);
		instancedStaticMeshComponent->UpdateInstanceTransform(node->instancedMeshIndex, transform);
	}
}

void ABattleGrid::HideNodes(TArray<int32>& indices)
{
	instancedStaticMeshComponent->MarkRenderStateDirty();

	for (int i = 0; i < indices.Num(); i++)
	{
		FGridNode* node = nodeMap.Find(indices[i]);
		GetNode(node->xIndex, node->yIndex)->bActive = false;

		FTransform transform;
		instancedStaticMeshComponent->GetInstanceTransform(indices[i], transform);
		transform.SetScale3D(nodeHiddenScale);
		instancedStaticMeshComponent->UpdateInstanceTransform(indices[i], transform);
	}
}

void ABattleGrid::UnhideNodes(TArray<FGridNode*>& nodesToUnhide)
{
	instancedStaticMeshComponent->MarkRenderStateDirty();

	for (FGridNode* node : nodesToUnhide)
	{
		node->bActive = true;

		FTransform transform;
		instancedStaticMeshComponent->GetInstanceTransform(node->instancedMeshIndex, transform);
		transform.SetScale3D(nodeVisibleScale);
		instancedStaticMeshComponent->UpdateInstanceTransform(node->instancedMeshIndex, transform);
	}
}

void ABattleGrid::UnhideNodes(TArray<int32>& indices)
{
	instancedStaticMeshComponent->MarkRenderStateDirty();

	for (int i = 0; i < indices.Num(); i++)
	{
		FGridNode* node = nodeMap.Find(indices[i]);
		GetNode(node->xIndex, node->yIndex)->bActive = true;

		FTransform transform;
		instancedStaticMeshComponent->GetInstanceTransform(indices[i], transform);
		transform.SetScale3D(nodeVisibleScale);
		instancedStaticMeshComponent->UpdateInstanceTransform(indices[i], transform);
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

void ABattleGrid::ChangeTurn()
{
	bPlayerTurn = !bPlayerTurn;
	bEnemyTurn = !bEnemyTurn;

	//Make sure turn bools don't overlap somehow
	check(bPlayerTurn != bEnemyTurn);

	if (bPlayerTurn)
	{
		UE_LOG(LogTemp, Warning, TEXT("PLAYER TURN"));
	} 
	else if(bEnemyTurn)
	{
		UE_LOG(LogTemp, Warning, TEXT("ENEMY TURN"));
	}

	allUnits.Empty();

	if (bEnemyTurn)
	{
		RepopulateUnitArray();
		SortUnitsByTurnSpeed();
	}

	//Reset player camera focus and AP
	APlayerUnit* player = Cast<APlayerUnit>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	player->selectedUnit = nullptr;

	if (bPlayerTurn)
	{
		player->currentActionPoints = player->maxActionPoints;
	}
}

void ABattleGrid::SortUnitsByTurnSpeed()
{
	for (int i = 1; i < allUnits.Num(); i++)
	{
		if (allUnits[i]->turnSpeed > allUnits[i - 1]->turnSpeed)
		{
			allUnits.Swap(i, i - 1);
			i = 1;
		}
	}
}

void ABattleGrid::RepopulateUnitArray()
{
	TArray<AActor*> outUnits;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AUnit::StaticClass(), outUnits);
	allUnits.Reserve(outUnits.Num());
	for (AActor* actor : outUnits)
	{
		AUnit* unit = Cast<AUnit>(actor);
		unit->bTurnFinished = false;
		unit->bSetToMove = false;
		allUnits.Add(unit);
	}
}
