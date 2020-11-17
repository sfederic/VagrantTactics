// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "EntranceKeys.h"
#include "MainGameInstance.generated.h"

class UIntuition;

//For handling in-game time and intuitions
UCLASS()
class VAGRANTTACTICS_API UMainGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UMainGameInstance();
	bool CheckTimeOfDayActivation(int hour, int minute);
	void ProgressTimeOfDay();

	FName previousLevelMovedFrom;

	//TODO: figure out whether time is constantly incrementing (Majora's Mask) or increments when an event occurs/finishes.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int currentHour;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int currentMinute;

	UPROPERTY() TArray<FName> entraceKeys;

	//there's a copy of all intuitions on player too, here for saving purposes
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TArray<UIntuition*> intuitions;
};
