// Fill out your copyright notice in the Description page of Project Settings.

#include "NPCUnit.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/WidgetComponent.h"
#include "SpeechWidget.h"

void ANPCUnit::BeginPlay()
{
	Super::BeginPlay();

	//Setup widgets
	TArray<UActorComponent*> outWidgets;
	GetComponents(UWidgetComponent::StaticClass(), outWidgets);
	for (UActorComponent* component : outWidgets)
	{
		if (component->GetName() == TEXT("HealthBar"))
		{
			healthBarWidgetComponent = Cast<UWidgetComponent>(component);
		}
		else if (component->GetName() == TEXT("WidgetSpeech"))
		{
			speechWidgetComponent = Cast<UWidgetComponent>(component);
			speechWidget = Cast<USpeechWidget>(speechWidgetComponent->GetUserWidgetObject());
		}
	}
}

void ANPCUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (pointToMoveTo)
	{
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
}
