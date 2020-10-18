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
			battleGrid->UnhideNodes(connectedNodeIndices);
			
			//For single GridActors that are spawned after level creation
			battleGrid->UnhideNode(battleGrid->GetNode(xIndex, yIndex));

			//Reset player camera focus on Destroy
			APlayerUnit* player = Cast<APlayerUnit>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
			player->ResetCameraFocusAndFOV();

			if (Tags.Contains(GameplayTags::Destructible))
			{
				UDestructibleComponent* dc = FindComponentByClass<UDestructibleComponent>();
				dc->ApplyDamage(1000.f, GetActorLocation(), FVector(FMath::RandRange(-1.f, 1.f)), 1000.f);

				healthbarWidgetComponent->SetHiddenInGame(true);

				SetLifeSpan(5.0f);
			}
			else
			{
				AUnit* unit = Cast<AUnit>(this);
				if (unit)
				{
					unit->particleFocusBeam->DestroyComponent();
				}

				Destroy();
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
