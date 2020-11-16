// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractTrigger.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerUnit.h"
#include "InteractWidget.h"
#include "InteractDetailsWidget.h"

AInteractTrigger::AInteractTrigger()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AInteractTrigger::BeginPlay()
{
	Super::BeginPlay();
	
	//Setup trigger
	box = FindComponentByClass<UBoxComponent>();
	box->OnComponentBeginOverlap.AddDynamic(this, &AInteractTrigger::OnBeginOverlap);
	box->OnComponentEndOverlap.AddDynamic(this, &AInteractTrigger::OnEndOverlap);
}

void AInteractTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInteractTrigger::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerUnit* player = Cast<APlayerUnit>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	player->overlappedInteractTrigger = this;
}

void AInteractTrigger::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APlayerUnit* player = Cast<APlayerUnit>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	player->overlappedInteractTrigger = nullptr;
	player->widgetInteract->interactText = FText();
	player->widgetInteract->RemoveFromViewport();
	player->widgetInteractDetails->RemoveFromViewport();
}
