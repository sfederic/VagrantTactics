// Fill out your copyright notice in the Description page of Project Settings.

#include "PickupItem.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerUnit.h"
#include "InteractWidget.h"

APickupItem::APickupItem()
{
	PrimaryActorTick.bCanEverTick = true;

}

void APickupItem::BeginPlay()
{
	Super::BeginPlay();
	
	box = FindComponentByClass<UBoxComponent>();
	box->OnComponentBeginOverlap.AddDynamic(this, &APickupItem::OnBeginOverlap);
	box->OnComponentEndOverlap.AddDynamic(this, &APickupItem::OnEndOverlap);
	//box->Deactivate();
}

void APickupItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APickupItem::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerUnit* player = Cast<APlayerUnit>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (player->holdingItemActor != this)
	{
		player->overlappedPickupItem = this;
		player->widgetInteract->interactText = interactText;
		player->widgetInteract->AddToViewport();
	}
}

void APickupItem::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APlayerUnit* player = Cast<APlayerUnit>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (player->holdingItemActor != this)
	{
		player->overlappedPickupItem = nullptr;
		player->widgetInteract->interactText = FText();
		player->widgetInteract->RemoveFromViewport();
	}
}
