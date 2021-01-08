// Fill out your copyright notice in the Description page of Project Settings.

#include "EventInstance.h"
#include "Kismet/GameplayStatics.h"
#include "MainGameInstance.h"
#include "Unit.h"
#include "BattleGrid.h"

AEventInstance::AEventInstance()
{
	PrimaryActorTick.bCanEverTick = false;

}

void AEventInstance::BeginPlay()
{
	Super::BeginPlay();
	
	UMainGameInstance* gameInstance = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	if (bActiveOverEntireHour)
	{
		if (gameInstance->currentHour == hourToActivate)
		{
			ActivateLinkedActors();
		}
		else
		{
			DeactivateLinkedActors();
		}
	}
	else
	{
		if (!gameInstance->CheckTimeOfDayActivation(hourToActivate, minuteToActivate))
		{
			DeactivateLinkedActors();
		}
		else if (gameInstance->CheckTimeOfDayActivation(hourToActivate, minuteToActivate))
		{
			ActivateLinkedActors();
		}
	}
}

void AEventInstance::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEventInstance::ActivateLinkedActors()
{
	for (AActor* actor : actorsToActivate)
	{
		actor->SetActorHiddenInGame(false);
		actor->SetActorEnableCollision(true);
		actor->SetActorTickEnabled(true);
	}
}

void AEventInstance::DeactivateLinkedActors()
{
	for (AActor* actor : actorsToActivate)
	{
		actor->Destroy();
		/*actor->SetActorHiddenInGame(true);
		actor->SetActorEnableCollision(false);
		actor->SetActorTickEnabled(false);

		AUnit* unit = Cast<AUnit>(actor);
		if (unit)
		{
			//unit->battleGrid->HideNode(unit->battleGrid->GetNode(unit->xIndex, unit->yIndex));
		}*/
	}
}