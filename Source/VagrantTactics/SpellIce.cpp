// Fill out your copyright notice in the Description page of Project Settings.

#include "SpellIce.h"
#include "PlayerUnit.h"
#include "Kismet/GameplayStatics.h"
#include "GridNode.h"
#include "BattleGrid.h"

void USpellIce::CastSpell(int x, int y, AGridActor* target)
{
	APlayerUnit* player = Cast<APlayerUnit>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	FGridNode* node = player->battleGrid->GetNode(x, y);
	GetWorld()->SpawnActor<AActor>(iceBlockToSpawn, FTransform(node->location));

	player->battleGrid->HideNode(node);
	UE_LOG(LogTemp, Warning, TEXT("Ice block cast"));
}
