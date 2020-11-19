// Fill out your copyright notice in the Description page of Project Settings.

#include "MainGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerUnit.h"

UMainGameInstance::UMainGameInstance()
{
	//Start day at 6 AM
	currentHour = 6;
	currentMinute = 0;
	playerStressPoints = 0;
}

bool UMainGameInstance::CheckTimeOfDayActivation(int hour, int minute)
{
	if (hour == currentHour)
	{
		if (minute == currentMinute)
		{
			return true;
		}
	}

	return false;
}

//Move the world time in intervals based on events
void UMainGameInstance::ProgressTimeOfDay()
{
	currentMinute += 15;
	if (currentMinute >= 60)
	{
		currentHour++;
		currentMinute = 0;
	}
}

void UMainGameInstance::CarryOverIntuitions()
{
	bGameOver = true;
	intuitions = intuitionsToCarryOver;
}
