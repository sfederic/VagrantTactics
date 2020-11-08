// Fill out your copyright notice in the Description page of Project Settings.

#include "NPCUnit.h"
#include "Kismet/KismetMathLibrary.h"

void ANPCUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bSetToMoveDuringConversation)
	{
		nextMoveLocation = pointToMoveTo->GetActorLocation();
		SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), pointToMoveTo->GetActorLocation()));

		bSetToMoveDuringConversation = false;
	}

	//Disolve actor when reaches destination
	if (GetActorLocation().Equals(pointToMoveTo->GetActorLocation()))
	{
		Destroy();
	}
}
