// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleInstance.h"
#include "Components/BoxComponent.h"
#include "Unit.h"
#include "Kismet/GameplayStatics.h"
#include "VagrantTacticsGameModeBase.h"
#include "BattleGrid.h"
#include "PlayerUnit.h"
#include "Blueprint/UserWidget.h"

ABattleInstance::ABattleInstance()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ABattleInstance::BeginPlay()
{
	Super::BeginPlay();
	
	box = FindComponentByClass<UBoxComponent>();
	box->OnComponentBeginOverlap.AddDynamic(this, &ABattleInstance::ActivateBattleOnOverlap);

	//Populate unit array
	unitsToActivateOnBattleStart.Empty();

	TArray<AActor*> outUnits;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AUnit::StaticClass(), outUnits);
	for (AActor* actor : outUnits)
	{
		unitsToActivateOnBattleStart.Add(Cast<AUnit>(actor));
	}

	numOfUnitsAlive = unitsToActivateOnBattleStart.Num();
}

void ABattleInstance::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//End battle if all units destroyed
	if(numOfUnitsAlive <= 0)
	{
		AVagrantTacticsGameModeBase* gameMode = Cast<AVagrantTacticsGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
		gameMode->activeBattleGrid->ActivateBattle();

		//Remove enemy turn order widget from viewport
		APlayerUnit* player = Cast<APlayerUnit>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		if (player)
		{
			if (player->widgetEnemyTurnOrder->IsInViewport())
			{
				player->widgetEnemyTurnOrder->RemoveFromViewport();
			}
		}

		Destroy();
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

	//Add enemy turn order widget to viewport
	APlayerUnit* player = Cast<APlayerUnit>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (player)
	{
		player->widgetEnemyTurnOrder = CreateWidget<UUserWidget>(GetWorld(), player->classEnemyTurnOrderWidget);
		player->widgetEnemyTurnOrder->AddToViewport();

		player->meshSword->SetVisibility(true);
	}

	box->DestroyComponent();
}
