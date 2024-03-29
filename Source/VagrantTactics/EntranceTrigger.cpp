// Fill out your copyright notice in the Description page of Project Settings.

#include "EntranceTrigger.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerUnit.h"
#include "InteractWidget.h"
#include "MainGameInstance.h"

AEntranceTrigger::AEntranceTrigger()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AEntranceTrigger::BeginPlay()
{
	Super::BeginPlay();
	
	box = FindComponentByClass<UBoxComponent>();
	box->OnComponentBeginOverlap.AddDynamic(this, &AEntranceTrigger::OnBeginOverlap);
	box->OnComponentEndOverlap.AddDynamic(this, &AEntranceTrigger::OnEndOverlap);
}

void AEntranceTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEntranceTrigger::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerUnit* player = Cast<APlayerUnit>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (player)
	{
		//Key in inventory
		UMainGameInstance* gameInstance = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		if (gameInstance->entraceKeys.Contains(entranceKey) || (entranceKey == TEXT("")))
		{
			if (player->widgetInteract)
			{
				if (!player->widgetInteract->IsInViewport())
				{
					player->overlappedEntrace = this;

					player->widgetInteract->interactText = FText::FromString(TEXT("Open"));
					player->widgetInteract->AddToViewport();
				}
			}
		}
		else //Entrance is locked
		{
			if (player->widgetInteract)
			{
				if (!player->widgetInteract->IsInViewport())
				{
					player->widgetInteract->interactText = FText::FromString(TEXT("Locked"));
					player->widgetInteract->AddToViewport();
				}
			}
		}
	}
}

void AEntranceTrigger::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APlayerUnit* player = Cast<APlayerUnit>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (player)
	{
		player->overlappedEntrace = nullptr;
		if (player->widgetInteract)
		{
			if (player->widgetInteract->IsInViewport())
			{
				player->widgetInteract->RemoveFromViewport();
			}
		}
	}
}
