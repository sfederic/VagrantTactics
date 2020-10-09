// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "VagrantTacticsGameModeBase.generated.h"

class AGridActor;
class ABattleGrid;
class AUnit;

//Base game mode for every level.
UCLASS()
class VAGRANTTACTICS_API AVagrantTacticsGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;

	UPROPERTY() TArray<AGridActor*> gridActors;
	UPROPERTY() TArray<AUnit*> units;
	ABattleGrid* activeBattleGrid;
};
