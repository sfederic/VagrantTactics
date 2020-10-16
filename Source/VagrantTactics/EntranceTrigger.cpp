// Fill out your copyright notice in the Description page of Project Settings.

#include "EntranceTrigger.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerUnit.h"

AEntranceTrigger::AEntranceTrigger()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AEntranceTrigger::BeginPlay()
{
	Super::BeginPlay();
	
	
}

void AEntranceTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEntranceTrigger::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerUnit* player = Cast<APlayerUnit>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	player->overlappedEntrace = this;
}

void AEntranceTrigger::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APlayerUnit* player = Cast<APlayerUnit>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	player->overlappedEntrace = nullptr;
}
