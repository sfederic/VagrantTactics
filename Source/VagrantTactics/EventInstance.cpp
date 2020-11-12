// Fill out your copyright notice in the Description page of Project Settings.

#include "EventInstance.h"
#include "Kismet/GameplayStatics.h"
#include "MainGameInstance.h"

AEventInstance::AEventInstance()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AEventInstance::BeginPlay()
{
	Super::BeginPlay();
	
	UMainGameInstance* gameInstance = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	if (timeOfDayActivation != gameInstance->currentTimeOfDay)
	{
		for (AActor* actor : actorsToActivate)
		{
			actor->SetActorHiddenInGame(true);
		}
	}
	else if (timeOfDayActivation == gameInstance->currentTimeOfDay)
	{
		for (AActor* actor : actorsToActivate)
		{
			actor->SetActorHiddenInGame(false);
		}
	}
}

void AEventInstance::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
