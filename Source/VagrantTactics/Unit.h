// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Unit.generated.h"

class ABattleGrid;

//Base class for all units.
UCLASS()
class VAGRANTTACTICS_API AUnit : public AActor
{
	GENERATED_BODY()
	
public:	
	AUnit();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	void ShowMovementPath(int movementPoints);
	
	ABattleGrid* battleGrid;

	UPROPERTY(VisibleAnywhere) int xIndex;
	UPROPERTY(VisibleAnywhere) int yIndex;

	UPROPERTY(EditAnywhere) int maxHealth;
	UPROPERTY(EditAnywhere) int currentHealth;
};
