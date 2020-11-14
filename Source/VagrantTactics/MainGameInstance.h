// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "EntranceKeys.h"
#include "MainGameInstance.generated.h"

//For handling in-game time and intuitions
UCLASS()
class VAGRANTTACTICS_API UMainGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UMainGameInstance();

	FName previousLevelMovedFrom;

	//TODO: figure out whether time is constantly incrementing (Majora's Mask) or increments when an event occurs/finishes.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int currentTimeOfDay;

	UPROPERTY() TArray<FName> entraceKeys;
};
