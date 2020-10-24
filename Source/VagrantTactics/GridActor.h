// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridNode.h" 
#include "IntuitionInterface.h"
#include "GridActor.generated.h"

class UHealthbarWidget;
class UWidgetComponent;

//Base class for static actors in level.
//Grid Actors can also have Intuitions connected to them (i.e player gains Intuition on inspection/kill)
UCLASS()
class VAGRANTTACTICS_API AGridActor : public AActor, public IIntuitionInterface
{
	GENERATED_BODY()
	
public:	
	AGridActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void AddIntuition() override;
	void SetIndices();

	//Every node an actor is connected to, even when scale larger than 1x1 on grid 
	UPROPERTY() TArray<int32> connectedNodeIndices;

	UHealthbarWidget* healthbarWidget;
	UWidgetComponent* healthbarWidgetComponent;

	class ABattleGrid* battleGrid;

	UPROPERTY(EditAnywhere, BlueprintReadOnly) int maxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) int currentHealth;

	UPROPERTY(VisibleAnywhere) int xIndex;
	UPROPERTY(VisibleAnywhere) int yIndex;

	UPROPERTY(EditAnywhere) bool bIsDestructible;

	//Sets if the actor is larger than 1x1 grid unit and swaps its grid indices creation in BattleGrid for a Sweep trace
	UPROPERTY(EditAnywhere) bool bLargerThanUnitSquare;
};
