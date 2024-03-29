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
#include "Kismet/KismetMathLibrary.h"
#include "ConversationInstance.h"

ABattleGrid::ABattleGrid()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ABattleGrid::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABattleGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//TODO: I fucking hate this code. Can't I use events somehow?
	//Iterate through enemy turn list based on highest speed first
	if (bEnemyTurn)
	{
		if (allUnits.Num() > 0 )
		{
			if (!allUnits[activeUnitIndex]->bTurnFinished && !allUnits[activeUnitIndex]->bCurrentlyMoving)
			{
				/*if (allUnits[activeUnitIndex]->bSetToUseSkill)
				{
					allUnits[activeUnitIndex]->UseSkill();
				}*/
				if (!allUnits[activeUnitIndex]->bSetToMove)
				{
					if (allUnits[activeUnitIndex]->pathNodes.Num() < 1)
					{
						allUnits[activeUnitIndex]->ShowMovementPath();
						allUnits[activeUnitIndex]->MoveTo(allUnits[activeUnitIndex]->FindTargetFocusNode());
					}
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
					allUnits[activeUnitIndex]->bSetToMove = false;
					allUnits[activeUnitIndex]->bTurnFinished = false;
					allUnits[activeUnitIndex]->bCurrentlyMoving = false;

					activeUnitIndex = 0;

					ChangeTurn();
				}
			}
		}
	}
}

FGridNode* ABattleGrid::GetNode(int x, int y)
{
	if (bIsSetAtWorldOrigin)
	{
		check(x < sizeX && x > -1);
		check(y < sizeY && y > -1);
	}

	return &rows[x].columns[y];
}

void ABattleGrid::Init()
{
	//Get all units and sort
	TArray<AActor*> outUnits;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AUnit::StaticClass(), outUnits);
	for (AActor* actor : outUnits)
	{
		AUnit* unit = Cast<AUnit>(actor);
		if (unit->bCanEnterBattle)
		{
			allUnits.Add(unit);
		}
	}

	numOfUnitsAlive = outUnits.Num();

	SortUnitsByTurnSpeed();


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

	FCollisionQueryParams params;
	params.AddIgnoredActors(outUnits);
	params.AddIgnoredActor(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

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

			//NOTE: careful with node.location and transform.GetLocation(). 0 is centered, transform has -50.f offset

			FTransform transform;
			transform.SetLocation(FVector((float)x * LevelGridValues::gridUnitDistance, (float)y * LevelGridValues::gridUnitDistance, -LevelGridValues::nodeHeightOffset));
			transform.SetScale3D(nodeVisibleScale);

			FHitResult hit;
			FVector startHit = transform.GetLocation() + FVector(0.f, 0.f, 1000.f);

			//Grid actor that will update connected node indices later
			AGridActor* gridActorToUpdate = nullptr;

			//Get the base foundation of the grid (lowest level)
			if(GetWorld()->LineTraceSingleByChannel(hit, startHit, transform.GetLocation(), ECC_WorldStatic, params))
			{
				node.bActive = true;
				transform.SetScale3D(nodeVisibleScale);
				transform.SetLocation(hit.ImpactPoint + FVector(0.f, 0.f, 1.f));
				FRotator rot = UKismetMathLibrary::MakeRotFromZ(hit.ImpactNormal);
				transform.SetRotation(FQuat(rot));
				node.location = FVector((float)x * LevelGridValues::gridUnitDistance, (float)y * LevelGridValues::gridUnitDistance, hit.ImpactPoint.Z + 50.f);

				AActor* hitActor = hit.GetActor();
				if (hitActor)
				{
					if (hitActor->ActorHasTag(GameplayTags::Player) || hitActor->ActorHasTag(GameplayTags::Unit)
						|| hitActor->ActorHasTag(GameplayTags::NonObstruct))
					{
						node.bActive = true;
						transform.SetScale3D(nodeVisibleScale);
						transform.SetLocation(node.location - FVector(0.f, 0.f, LevelGridValues::nodeHeightOffset));
						node.location = FVector((float)x * LevelGridValues::gridUnitDistance, (float)y * LevelGridValues::gridUnitDistance, 0.f);
					}
					else if (hitActor->ActorHasTag(GameplayTags::Obstruct))
					{
						node.bActive = false;
						transform.SetScale3D(nodeHiddenScale);
					}

					for (AGridActor* gridActor : allGridActors)
					{
						uint32 hitActorID = hitActor->GetUniqueID();
						uint32 gridActorID = gridActor->GetUniqueID();
						if (gridActorID == hitActorID)
						{
							gridActorToUpdate = gridActor;
							node.bActive = false;
							transform.SetScale3D(nodeHiddenScale);
						}
					}
				}
			}


			//Check for hole in floor
			FHitResult holeHit;
			if (!GetWorld()->LineTraceSingleByChannel(holeHit, transform.GetLocation(), 
				transform.GetLocation() - FVector(0.f, 0.f, 100.f), ECC_WorldStatic))
			{
				node.bActive = false;
				transform.SetScale3D(nodeHiddenScale);
			}

			//Grid node mesh instance
			int32 instancedMeshIndex = gridMesh->AddInstance(transform);

			//Update actor indices
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

	//Setup all inactive nodes that all gridactors are on
	for (AGridActor* gridActor : allGridActors)
	{
		//Keep note that in some cutscene instances, grid actors might be off the level's battle grid
		//Eg. NPC giving a speech on a balcony
		if (gridActor->xIndex < this->sizeX)
		{
			if (gridActor->yIndex < this->sizeY)
			{
				FGridNode* node = this->GetNode(gridActor->xIndex, gridActor->yIndex);
				if (node)
				{
					node->bActive = false;
				}
			}
		}
		
	}
}

//Both starts and ends battle
void ABattleGrid::ActivateBattle()
{
	bBattleActive = !bBattleActive;

	//Player widgets
	APlayerUnit* player = Cast<APlayerUnit>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	if (bBattleActive)
	{
		//End all active conversations 
		TArray<AActor*> outConversationInstances;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AConversationInstance::StaticClass(), outConversationInstances);
		for (AActor* actor : outConversationInstances)
		{
			actor->Destroy();
		}

		GEngine->AddOnScreenDebugMessage(0, 2.0f, FColor::Red, TEXT("Battle Activated"));

		player->widgetActionPoints->AddToViewport();

		gridMesh->SetHiddenInGame(false);

		//Show all unit actor health bars on battle start (don't worry about destructibles)
		TArray<AActor*> outUnits;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AUnit::StaticClass(), outUnits);
		for (AActor* actor : outUnits)
		{
			AUnit* unit = Cast<AUnit>(actor);
			if (unit)
			{
				unit->ShowUnitFocus();

				if (unit->bIsDestructible)
				{
					//NOTE: This was causing a lot of trouble before when the component was accesed without Find
					UWidgetComponent* wc = unit->FindComponentByClass<UWidgetComponent>();
					if (wc)
					{
						wc->SetHiddenInGame(false);
					}
				}
			}
		}
	}
	else if (!bBattleActive)
	{
		GEngine->AddOnScreenDebugMessage(0, 2.0f, FColor::Blue, TEXT("Battle Ended"));

		player->widgetActionPoints->RemoveFromViewport();
		player->bWeaponUnsheathed = false;

		gridMesh->SetHiddenInGame(true);

		ResetAllNodeValues();

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
			if (node->location.Z < (centerNode->location.Z + 100.f))
			{
				node->bClosed = true;
				node->parentNode = centerNode;
				outNodes.Add(node);
			}
		}
	}

	//-X
	if (currentX > 0)
	{
		FGridNode* node = &rows[currentX - 1].columns[currentY];
		if (!node->bClosed && node->bActive)
		{
			if (node->location.Z < (centerNode->location.Z + 100.f))
			{
				node->bClosed = true;
				node->parentNode = centerNode;
				outNodes.Add(node);
			}
		}
	}

	//+Y
	if (currentY < (sizeY - 1))
	{
		FGridNode* node = &rows[currentX].columns[currentY + 1];
		if (!node->bClosed && node->bActive)
		{
			if (node->location.Z < (centerNode->location.Z + 100.f))
			{
				node->bClosed = true;
				node->parentNode = centerNode;
				outNodes.Add(node);
			}
		}
	}

	//-Y
	if (currentY > 0)
	{
		FGridNode* node = &rows[currentX].columns[currentY - 1];
		if (!node->bClosed && node->bActive)
		{
			if (node->location.Z < (centerNode->location.Z + 100.f))
			{
				node->bClosed = true;
				node->parentNode = centerNode;
				outNodes.Add(node);
			}
		}
	}
}

void ABattleGrid::HideAllNodes()
{
	//gridMesh->SetHiddenInGame(true);
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

void ABattleGrid::ActivateAllNodes()
{
	for (int x = 0; x < sizeX; x++)
	{
		for (int y = 0; y < sizeY; y++)
		{
			rows[x].columns[y].bActive = true;
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

void ABattleGrid::UnhideNodes(TArray<int32>& indices, bool bShow, bool bRecalculateLocation)
{
	gridMesh->MarkRenderStateDirty();

	FCollisionQueryParams recalculateIgnores;
	if (bRecalculateLocation)
	{
		TArray<AActor*> outDestructibleActors;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), GameplayTags::Destructible, outDestructibleActors);
		recalculateIgnores.AddIgnoredActors(outDestructibleActors);
	}

	for (int i = 0; i < indices.Num(); i++)
	{
		FGridNode* node = nodeMap.Find(indices[i]);
		GetNode(node->xIndex, node->yIndex)->bActive = true;
		GetNode(node->xIndex, node->yIndex)->location.Z = 0.f;

		if (bShow)
		{
			FTransform transform;
			gridMesh->GetInstanceTransform(indices[i], transform);
			transform.SetScale3D(nodeVisibleScale);

			if (bRecalculateLocation)
			{
				FHitResult newLocationHit;
				FVector startHit = transform.GetLocation() + FVector(0.f, 0.f, 1000.f);
				FVector endHit = transform.GetLocation() - FVector(0.f, 0.f, 1000.f);
				if (GetWorld()->LineTraceSingleByChannel(newLocationHit, startHit, endHit, ECC_WorldStatic, recalculateIgnores))
				{
					FVector newNodeLocation = newLocationHit.ImpactPoint;
					newNodeLocation.Z += 1.f;
					transform.SetLocation(newNodeLocation);
					GetNode(node->xIndex, node->yIndex)->location = newLocationHit.ImpactPoint + FVector(0.f, 0.f, 50.f);
				}
			}

			gridMesh->UpdateInstanceTransform(indices[i], transform);
		}
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

	//HideAllNodes();

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
