// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleInstance.h"
#include "Components/BoxComponent.h"
#include "Unit.h"
#include "Kismet/GameplayStatics.h"
#include "VagrantTacticsGameModeBase.h"
#include "BattleGrid.h"

ABattleInstance::ABattleInstance()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ABattleInstance::BeginPlay()
{
	Super::BeginPlay();
	
	box = FindComponentByClass<UBoxComponent>();
	box->OnComponentBeginOverlap.AddDynamic(this, &ABattleInstance::ActivateBattleOnOverlap);
}

void ABattleInstance::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//End battle if all units destroyed
	for(int i = 0; i < unitsToActivateOnBattleStart.Num(); i++)
	{
		if (unitsToActivateOnBattleStart[i] != nullptr)
		{
			break;
		}

		if (i == (unitsToActivateOnBattleStart.Num() - 1))
		{
			AVagrantTacticsGameModeBase* gameMode = Cast<AVagrantTacticsGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
			gameMode->activeBattleGrid->ActivateBattle();
			Destroy();
		}
	}
}

void ABattleInstance::ActivateBattleOnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AVagrantTacticsGameModeBase* gameMode = Cast<AVagrantTacticsGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	gameMode->activeBattleGrid->ActivateBattle();

	for (int i = 0; i < unitsToActivateOnBattleStart.Num(); i++)
	{
		unitsToActivateOnBattleStart[i]->bInBattle = true;
	}

	/*for (AUnit* unit : unitsToActivateOnBattleStart)
	{
		unit->bInBattle = true;
	}*/

	box->DestroyComponent();
}
