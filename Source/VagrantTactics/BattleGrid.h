// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridNode.h"
#include "BattleGrid.generated.h"

class AUnit;

struct GridRow
{
	TArray<FGridNode> columns;
};

//Grid that spawns around enemies and player for battle. BattleGrid always needs to be at world origin (0, 0, 0). 
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
	void Init(); //Init() exists because of BeginPlay() order not being a thing. Have to rely on GameMode
	void ActivateBattle();
	void GetNeighbouringNodes(FGridNode* centerNode, TArray<FGridNode*>& outNodes);
	void HideAllNodes();
	void ShowAllNodes();
	void ActivateAllNodes();
	void HideNode(FGridNode* node);
	void UnhideNode(FGridNode* node);
	void HideNodes(TArray<FGridNode*>& nodesToHide);
	void HideNodes(TArray<int32>& indices);
	void UnhideNodes(TArray<FGridNode*>& nodesToUnhide);
	void UnhideNodes(TArray<int32>& indices, bool bShow);
	void ResetAllNodeValues();
	void ChangeTurn();
	void SortUnitsByTurnSpeed();
	void RepopulateUnitArray();
	
	//Both functions are different from Hide/UnhideAllNodes as they're taking into account bActive from Node
	void ToggleGridOn(); 
	void ToggleGridOff(); 

	TArray<GridRow> rows;

	//Keep in mind because of the USTRUCT() serialization fuckery, have to manually copy GridNode values across through Find()s
	UPROPERTY() TMap<int32, FGridNode> nodeMap;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TArray<AUnit*> allUnits;
	int activeUnitIndex;

	FVector nodeVisibleScale = FVector(0.95f);
	FVector nodeHiddenScale = FVector(0.f);

	class UInstancedStaticMeshComponent* gridMesh;

	UPROPERTY(EditAnywhere) int sizeX;
	UPROPERTY(EditAnywhere) int sizeY;

	UPROPERTY(VisibleAnywhere) bool bBattleActive = false;
	UPROPERTY(VisibleAnywhere) bool bPlayerTurn = true;
	UPROPERTY(VisibleAnywhere) bool bEnemyTurn;

	//Testing to see if battlegrid can be places at other points to reuse levels
	UPROPERTY(EditAnywhere) bool bIsSetAtWorldOrigin = true;
};
