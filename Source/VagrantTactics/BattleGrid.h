// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridNode.h"
#include "BattleGrid.generated.h"

struct GridRow
{
	TArray<FGridNode> columns;
};

//Grid that spawns around enemies and player for battle.
UCLASS()
class VAGRANTTACTICS_API ABattleGrid : public AActor
{
	GENERATED_BODY()
	
public:	
	ABattleGrid();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	FGridNode* GetNode(int x, int y);
	void ActivateBattle();
	void GetNeighbouringNodes(FGridNode* centerNode, TArray<FGridNode*>& outNodes);
	void HideNodes(TArray<FGridNode*> nodesToHide);
	void UnhideNodes(TArray<FGridNode*> nodesToUnhide);
	void ResetAllNodes();

	TArray<GridRow> rows;

	class UInstancedStaticMeshComponent* instancedStaticMeshComponent;

	UPROPERTY(EditAnywhere) int sizeX;
	UPROPERTY(EditAnywhere) int sizeY;

	UPROPERTY(VisibleAnywhere) bool bBattleActive;
};
