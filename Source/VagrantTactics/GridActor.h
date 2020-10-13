// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridNode.h" 
#include "GridActor.generated.h"

class UHealthbarWidget;
class UWidgetComponent;

//Base class for static actors in level.
UCLASS()
class VAGRANTTACTICS_API AGridActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AGridActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

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
};
