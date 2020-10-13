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
#include "Components/StaticMeshComponent.h"

AGridActor::AGridActor()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AGridActor::BeginPlay()
{
	Super::BeginPlay();
	
	currentHealth = maxHealth;

	xIndex = FMath::RoundToInt(GetActorLocation().X / LevelGridValues::gridUnitDistance);
	yIndex = FMath::RoundToInt(GetActorLocation().Y / LevelGridValues::gridUnitDistance);

	//Setup health bar
	healthbarWidgetComponent = FindComponentByClass<UWidgetComponent>();
	healthbarWidgetComponent->SetHiddenInGame(true);
	healthbarWidget = Cast<UHealthbarWidget>(healthbarWidgetComponent->GetUserWidgetObject());
	healthbarWidget->attachedUnit = this;
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
			
			//Reset player camera focus on Destroy
			APlayerUnit* player = Cast<APlayerUnit>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
			player->ResetCameraFocusAndFOV();

			if (Tags.Contains(GameplayTags::Destructible))
			{
				//Since Apex is messing with physics, have to have a proxy static mesh for now to work with grid node hiding.
				//Component is being destroyed to remove physics mishaps, set hidden by default
				FindComponentByClass<UStaticMeshComponent>()->DestroyComponent();

				UDestructibleComponent* dc = FindComponentByClass<UDestructibleComponent>();
				dc->ApplyDamage(1000.f, GetActorLocation(), FVector(FMath::RandRange(-1.f, 1.f)), 1000.f);

				healthbarWidgetComponent->SetHiddenInGame(true);

				SetLifeSpan(5.0f);
			}
			else
			{
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
