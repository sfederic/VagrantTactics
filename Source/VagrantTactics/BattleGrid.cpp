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
	gridMesh = FindComponentByClass<UInstancedStaticMeshComponent>();
	check(gridMesh);
	
	TArray<AActor*> outGridActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGridActor::StaticClass(), outGridActors);
	TArray<AGridActor*> allGridActors;
	for (AActor* actor : outGridActors)
	{
		AGridActor* gridActor = Cast<AGridActor>(actor);
		gridActor->SetIndices();
		allGridActors.Add(gridActor);
	}

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
			FVector startHit = transform.GetLocation() + FVector(0.f, 0.f, 1000.f);

			//Grid actor that will update connected node indices later
			AGridActor* gridActorToUpdate = nullptr;

			if (GetWorld()->SweepSingleByChannel(hit, startHit, transform.GetLocation() + FVector(0.f, 0.f, 50.f), FQuat::Identity,
				ECC_WorldStatic, FCollisionShape::MakeBox(FVector(32.f))))
			{
				node.bActive = false;
				transform.SetScale3D(nodeHiddenScale);

				//The weird if checks here are to check for BSP geom
				AActor* hitActor = hit.GetActor();
				if (hitActor)
				{
					if (hitActor->ActorHasTag(GameplayTags::Player) || hitActor->ActorHasTag(GameplayTags::Unit)
						|| hitActor->ActorHasTag(GameplayTags::NonObstruct))
					{
						node.bActive = true;
						transform.SetScale3D(nodeVisibleScale);
					}
				}
			}

			for(AGridActor* gridActor : allGridActors)
			{
				if ((gridActor->xIndex == x) && (gridActor->yIndex == y))
				{
					if (!gridActor->Tags.Contains(GameplayTags::Player))
					{
						//Setting the scale for the instanced is the only way for now to disable their collision and visibility.
						transform.SetScale3D(nodeHiddenScale);
						node.bActive = false;

						gridActorToUpdate = gridActor;
						break;
					}
				}
			}

			//Deal with platforms and holes in floor
			{
				FHitResult platformHit;
				FVector startPlatformHit = transform.GetLocation() + FVector(0.f, 0.f, 1000.f);
				//FVector endPlatformHit = transform.GetLocation() - FVector(0.f, 0.f, -200.f);
				if (GetWorld()->LineTraceSingleByChannel(platformHit, startPlatformHit, transform.GetLocation(), ECC_WorldStatic, params))
				{
					if (platformHit.GetActor())
					{
						if (platformHit.GetActor()->Tags.Contains(GameplayTags::Platform))
						{
							transform.SetLocation(platformHit.ImpactPoint + FVector(0.f, 0.f, 5.f));
							transform.SetScale3D(nodeVisibleScale);
							node.location = transform.GetLocation() + FVector(0.f, 0.f, LevelGridValues::nodeHeightOffset);
							node.bActive = true;
						}
					}
				}
				else
				{
					//Hole in level floor
					/*transform.SetLocation(FVector((float)x, (float)y, 0.f));
					transform.SetScale3D(nodeHiddenScale);
					node.location = transform.GetLocation();
					node.bActive = false;*/
				}
			}


			if (hit.GetActor())
			{
				AGridActor* gridActor = Cast<AGridActor>(hit.GetActor());
				if(gridActor)
				{
					if (gridActor->bLargerThanUnitSquare)
					{
						gridActorToUpdate = gridActor;
						node.bActive = false;
						transform.SetScale3D(nodeHiddenScale);
					}
				}
			}


			int32 instancedMeshIndex = gridMesh->AddInstance(transform);


			if (gridActorToUpdate)
			{
				gridActorToUpdate->connectedNodeIndices.Add(instancedMeshIndex);
			}

			node.instancedMeshIndex = instancedMeshIndex;
			rows[x].columns.Add(node);

			//Add node to map through instance mesh index
			nodeMap.Add(instancedMeshIndex, rows[x].columns[y]);
		}
	}
}

void ABattleGrid::ActivateBattle()
{
	bBattleActive = !bBattleActive;

	GEngine->AddOnScreenDebugMessage(0, 2.0f, FColor::Red, TEXT("Battle Activated"));

	//Player widgets
	APlayerUnit* player = Cast<APlayerUnit>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	if (bBattleActive)
	{
		player->widgetActionPoints->AddToViewport();

		gridMesh->SetHiddenInGame(false);

		//Show all grid actor health bars on battle start
		TArray<AActor*> outGridActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGridActor::StaticClass(), outGridActors);
		for (AActor* actor : outGridActors)
		{
			AGridActor* gridActor = Cast<AGridActor>(actor);
			if (gridActor->bIsDestructible)
			{
				//NOTE: This was causing a lot of trouble before when the component was accesed without Find
				UWidgetComponent* wc = gridActor->FindComponentByClass<UWidgetComponent>();
				if (wc)
				{
					wc->SetHiddenInGame(false);
				}
			}
		}
	}
	else if (!bBattleActive)
	{
		player->widgetActionPoints->RemoveFromViewport();

		gridMesh->SetHiddenInGame(true);

		//Hide all grid actor health bars on battle end
		TArray<AActor*> outGridActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGridActor::StaticClass(), outGridActors);
		for (AActor* actor : outGridActors)
		{
			AGridActor* gridActor = Cast<AGridActor>(actor);
			if (gridActor->bIsDestructible)
			{
				UWidgetComponent* wc = gridActor->FindComponentByClass<UWidgetComponent>();
				if (wc)
				{
					wc->SetHiddenInGame(true);
				}
			}
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

void ABattleGrid::HideAllNodes()
{
	gridMesh->SetHiddenInGame(true);
	gridMesh->MarkRenderStateDirty();

	for (int x = 0; x < sizeX; x++)
	{
		for (int y = 0; y < sizeY; y++)
		{
			int32 meshIndex = rows[x].columns[y].instancedMeshIndex;

			FTransform transform;
			gridMesh->GetInstanceTransform(meshIndex, transform);
			transform.SetScale3D(nodeHiddenScale);
			gridMesh->UpdateInstanceTransform(meshIndex, transform);
		}
	}
}

void ABattleGrid::ShowAllNodes()
{
	gridMesh->SetHiddenInGame(false);
	gridMesh->MarkRenderStateDirty();

	for (int x = 0; x < sizeX; x++)
	{
		for (int y = 0; y < sizeY; y++)
		{
			int32 meshIndex = rows[x].columns[y].instancedMeshIndex;

			FTransform transform;
			gridMesh->GetInstanceTransform(meshIndex, transform);
			transform.SetScale3D(nodeVisibleScale);
			gridMesh->UpdateInstanceTransform(meshIndex, transform);
		}
	}
}

void ABattleGrid::HideNode(FGridNode* node)
{
	gridMesh->MarkRenderStateDirty();

	node->bActive = false;

	FTransform transform;
	gridMesh->GetInstanceTransform(node->instancedMeshIndex, transform);
	transform.SetScale3D(nodeHiddenScale);
	gridMesh->UpdateInstanceTransform(node->instancedMeshIndex, transform);
}

void ABattleGrid::UnhideNode(FGridNode* node)
{
	gridMesh->MarkRenderStateDirty();

	node->bActive = true;

	FTransform transform;
	gridMesh->GetInstanceTransform(node->instancedMeshIndex, transform);
	transform.SetScale3D(nodeVisibleScale);
	gridMesh->UpdateInstanceTransform(node->instancedMeshIndex, transform);
}

void ABattleGrid::HideNodes(TArray<FGridNode*>& nodesToHide)
{
	//Instance meshes need to have render dirty flag set to update transform.
	gridMesh->MarkRenderStateDirty();

	for (FGridNode* node : nodesToHide)
	{
		node->bActive = false;

		FTransform transform;
		gridMesh->GetInstanceTransform(node->instancedMeshIndex, transform);
		transform.SetScale3D(nodeHiddenScale);
		gridMesh->UpdateInstanceTransform(node->instancedMeshIndex, transform);
	}
}

void ABattleGrid::HideNodes(TArray<int32>& indices)
{
	gridMesh->MarkRenderStateDirty();

	for (int i = 0; i < indices.Num(); i++)
	{
		FGridNode* node = nodeMap.Find(indices[i]);
		GetNode(node->xIndex, node->yIndex)->bActive = false;

		FTransform transform;
		gridMesh->GetInstanceTransform(indices[i], transform);
		transform.SetScale3D(nodeHiddenScale);
		gridMesh->UpdateInstanceTransform(indices[i], transform);
	}
}

void ABattleGrid::UnhideNodes(TArray<FGridNode*>& nodesToUnhide)
{
	gridMesh->MarkRenderStateDirty();

	for (FGridNode* node : nodesToUnhide)
	{
		node->bActive = true;

		FTransform transform;
		gridMesh->GetInstanceTransform(node->instancedMeshIndex, transform);
		transform.SetScale3D(nodeVisibleScale);
		gridMesh->UpdateInstanceTransform(node->instancedMeshIndex, transform);
	}
}

void ABattleGrid::UnhideNodes(TArray<int32>& indices)
{
	gridMesh->MarkRenderStateDirty();

	for (int i = 0; i < indices.Num(); i++)
	{
		FGridNode* node = nodeMap.Find(indices[i]);
		GetNode(node->xIndex, node->yIndex)->bActive = true;

		FTransform transform;
		gridMesh->GetInstanceTransform(indices[i], transform);
		transform.SetScale3D(nodeVisibleScale);
		gridMesh->UpdateInstanceTransform(indices[i], transform);
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
	if (!bBattleActive)
	{
		UE_LOG(LogTemp, Warning, TEXT("Battle not active. Cannot end turn."));
		return;
	}

	bPlayerTurn = !bPlayerTurn;
	bEnemyTurn = !bEnemyTurn;

	HideAllNodes();

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

void ABattleGrid::ToggleGridOn()
{
	gridMesh->SetHiddenInGame(false);
	gridMesh->MarkRenderStateDirty();

	for (int x = 0; x < sizeX; x++)
	{
		for (int y = 0; y < sizeY; y++)
		{
			int32 meshIndex = rows[x].columns[y].instancedMeshIndex;

			if (rows[x].columns[y].bActive)
			{
				FTransform transform;
				gridMesh->GetInstanceTransform(meshIndex, transform);
				transform.SetScale3D(nodeVisibleScale);
				gridMesh->UpdateInstanceTransform(meshIndex, transform);
			}
		}
	}
}

void ABattleGrid::ToggleGridOff()
{
	gridMesh->SetHiddenInGame(true);
	gridMesh->MarkRenderStateDirty();

	for (int x = 0; x < sizeX; x++)
	{
		for (int y = 0; y < sizeY; y++)
		{
			int32 meshIndex = rows[x].columns[y].instancedMeshIndex;

			if (rows[x].columns[y].bActive)
			{
				FTransform transform;
				gridMesh->GetInstanceTransform(meshIndex, transform);
				transform.SetScale3D(nodeHiddenScale);
				gridMesh->UpdateInstanceTransform(meshIndex, transform);
			}
		}
	}
}
