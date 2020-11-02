// Fill out your copyright notice in the Description page of Project Settings.

#include "GridActor.h"
#include "LevelGridValues.h"
#include "BattleGrid.h"
#include "PlayerUnit.h"
#include "Kismet/GameplayStatics.h"
#include "GameplayTags.h"
#include "DestructibleComponent.h"
#include "HealthbarWidget.h"
#include "Components/WidgetComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Unit.h"
#include "GameStatics.h"
#include "VagrantTacticsGameModeBase.h"
#include "IntuitionComponent.h"
#include "BattleInstance.h"

void AGridActor::AddIntuition()
{

}

AGridActor::AGridActor()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AGridActor::BeginPlay()
{
	Super::BeginPlay();
	
	//Setup battlegrid if actor is spawned after level begin
	if (battleGrid == nullptr)
	{
		AVagrantTacticsGameModeBase* gameMode = Cast<AVagrantTacticsGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
		battleGrid = gameMode->activeBattleGrid;
	}

	currentHealth = maxHealth;

	xIndex = FMath::RoundToInt(GetActorLocation().X / LevelGridValues::gridUnitDistance);
	yIndex = FMath::RoundToInt(GetActorLocation().Y / LevelGridValues::gridUnitDistance);

	//Setup health bar
	if (bIsDestructible)
	{
		healthbarWidgetComponent = FindComponentByClass<UWidgetComponent>();
		if (healthbarWidgetComponent)
		{
			healthbarWidgetComponent->SetHiddenInGame(true);
			healthbarWidget = Cast<UHealthbarWidget>(healthbarWidgetComponent->GetUserWidgetObject());
			healthbarWidget->attachedUnit = this;
		}
	}
}

void AGridActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	xIndex = FMath::RoundToInt(GetActorLocation().X / LevelGridValues::gridUnitDistance);
	yIndex = FMath::RoundToInt(GetActorLocation().Y / LevelGridValues::gridUnitDistance);

	if (bIsDestructible)
	{
		if (currentHealth <= 0)
		{
			battleGrid->UnhideNodes(connectedNodeIndices, false);

			//For single GridActors that are spawned after level creation
			//battleGrid->UnhideNode(battleGrid->GetNode(xIndex, yIndex));
			battleGrid->GetNode(xIndex, yIndex)->bActive = true;

			//Reset player camera focus on Destroy
			APlayerUnit* player = Cast<APlayerUnit>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
			player->ResetCameraFocusAndFOV();

			if (Tags.Contains(GameplayTags::Destructible))
			{
				UDestructibleComponent* dc = FindComponentByClass<UDestructibleComponent>();
				dc->ApplyDamage(100.f, GetActorLocation(), FVector(FMath::RandRange(-1.f, 1.f)), 100.f);

				healthbarWidgetComponent->SetHiddenInGame(true);

				Tags.Empty();
				SetLifeSpan(5.0f);
			}
			else
			{
				AUnit* unit = Cast<AUnit>(this);
				if (unit)
				{
					unit->particleFocusBeam->DestroyComponent();
					battleGrid->HideNodes(unit->movementPathNodes);
					battleGrid->ResetAllNodeValues();
					
					TArray<AActor*> outBattleInstance;
					UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABattleInstance::StaticClass(), outBattleInstance);
					ABattleInstance* battleInstance = Cast<ABattleInstance>(outBattleInstance[0]);
					battleInstance->numOfUnitsAlive--;
				}

				Destroy();
			}

			//Check for Intuition
			UIntuitionComponent* intuitionComponent = FindComponentByClass<UIntuitionComponent>();
			if (intuitionComponent)
			{
				UIntuition* newIntuition = NewObject<UIntuition>(player, intuitionComponent->intuitionClass);
				IIntuitionInterface* intuitionInterface = Cast<IIntuitionInterface>(newIntuition);
				if (intuitionInterface)
				{
					intuitionInterface->AddIntuition();
				}

				player->intuitions.Add(newIntuition);
				GEngine->AddOnScreenDebugMessage(0, 2.0f, FColor::Green, TEXT("Intuition added"));
			}

			bIsDestructible = false;
		}
	}
}

void AGridActor::SetIndices()
{
	xIndex = FMath::RoundToInt(GetActorLocation().X / LevelGridValues::gridUnitDistance);
	yIndex = FMath::RoundToInt(GetActorLocation().Y / LevelGridValues::gridUnitDistance);
}
