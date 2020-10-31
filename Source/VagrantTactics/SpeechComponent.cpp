// Fill out your copyright notice in the Description page of Project Settings.

#include "SpeechComponent.h"
#include "Components/WidgetComponent.h"
#include "SpeechWidget.h"
#include "ConversationTable.h"
#include "TimerManager.h"

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

void USpeechComponent::ShowDialogue()
{
	widgetComponent->SetHiddenInGame(false);
	speechWidget = Cast<USpeechWidget>(widgetComponent->GetUserWidgetObject());
	speechWidget->dialogueLine = dialogueText;

	//Set timer to hide dialogue
	FTimerHandle handle;
	GetOwner()->GetWorldTimerManager().SetTimer(handle, this, &USpeechComponent::HideDialogue, 3.0f, false);
}

void USpeechComponent::HideDialogue()
{
	widgetComponent->SetHiddenInGame(true);
	speechWidget->dialogueLine = FText();
}
