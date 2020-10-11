// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridActor.h"
#include "GridNode.h"
#include "Unit.generated.h"

//Base class for all units.
UCLASS()
class VAGRANTTACTICS_API AUnit : public AGridActor
{
	GENERATED_BODY()
	
public:	
	AUnit();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	void ShowMovementPath(int movementPoints);
	void MoveTo(FGridNode destinationNode);

	UPROPERTY() TArray<FGridNode> movementPathNodes; //Movement nodes are the preview nodes shown on click
	UPROPERTY() TArray<FGridNode> pathNodes; //Path nodes are the final path the unit will take in Tick()

	FVector nextMoveLocation;
	int movementPathNodeIndex = 0;

	UPROPERTY(EditAnywhere) float moveSpeed;

	UPROPERTY(EditAnywhere) int maxMovementPoints;
	UPROPERTY(EditAnywhere) int currentMovementPoints;

	UPROPERTY(VisibleAnywhere) bool bSetToMove = false;
};
