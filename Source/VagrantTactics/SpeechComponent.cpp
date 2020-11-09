// Fill out your copyright notice in the Description page of Project Settings.

#include "SpeechComponent.h"
#include "Components/WidgetComponent.h"
#include "SpeechWidget.h"
#include "ConversationTable.h"
#include "TimerManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

USpeechComponent::USpeechComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USpeechComponent::BeginPlay()
{
	Super::BeginPlay();
	
	widgetComponent = GetOwner()->FindComponentByClass<UWidgetComponent>();
	if (widgetComponent)
	{
		speechWidget = Cast<USpeechWidget>(widgetComponent->GetUserWidgetObject());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Widget component not set on %s. SpeechComponent present."), *GetOwner()->GetName());
	}
}

void USpeechComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void USpeechComponent::ShowDialogue(bool bStartCombat)
{
	widgetComponent->SetHiddenInGame(false);
	speechWidget = Cast<USpeechWidget>(widgetComponent->GetUserWidgetObject());
	if (bStartCombat)
	{
		speechWidget->dialogueLine = startCombatText;
	}
	else
	{
		speechWidget->dialogueLine = dialogueText;
	}

	//Rotate towards player (like in the old SNES games)
	APawn* player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (player)
	{
		GetOwner()->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetOwner()->GetActorLocation(), player->GetActorLocation()));
	}

	//Set timer to hide dialogue
	FTimerHandle handle;
	GetOwner()->GetWorldTimerManager().SetTimer(handle, this, &USpeechComponent::HideDialogue, 3.0f, false);
}

void USpeechComponent::HideDialogue()
{
	widgetComponent->SetHiddenInGame(true);
	speechWidget->dialogueLine = FText();
}
