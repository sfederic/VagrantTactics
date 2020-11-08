// Fill out your copyright notice in the Description page of Project Settings.

#include "ConversationInstance.h"
#include "NPCUnit.h"
#include "TimerManager.h"
#include "ConversationTable.h"
#include "Components/WidgetComponent.h"
#include "SpeechWidget.h"

AConversationInstance::AConversationInstance()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AConversationInstance::BeginPlay()
{
	Super::BeginPlay();
	
	FString contextString;
	conversationTable->GetAllRows(contextString, conversationRows);

	FTimerHandle handle;
	GetWorldTimerManager().SetTimer(handle, this, &AConversationInstance::ShowNextDialogueLine, initialTimeForConversationStart, false);
}

void AConversationInstance::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AConversationInstance::ShowNextDialogueLine()
{
	if (conversationOrderIndex >= conversationRows.Num())
	{
		if (conversationOrderIndex > 0)
		{
			npcConversationOrder[conversationOrderIndex - 1]->FindComponentByClass<UWidgetComponent>()->SetHiddenInGame(true);
		}

		UE_LOG(LogTemp, Warning, TEXT("%s conversation finished."), *this->GetName());

		//Testing case for moving NPCs at end of conversation
		for (ANPCUnit* npc : npcConversationOrder)
		{
			npc->bSetToMoveDuringConversation = true;
		}

		Destroy();
		return;
	}

	FTimerHandle handle;
	GetWorldTimerManager().SetTimer(handle, this, &AConversationInstance::ShowNextDialogueLine, timeBetweenTextChanges, false);

	if (npcConversationOrder.Num() > 0)
	{
		//Don't hide previous speech widget if not first 
		if (conversationOrderIndex > 0)
		{
			npcConversationOrder[conversationOrderIndex - 1]->FindComponentByClass<UWidgetComponent>()->SetHiddenInGame(true);
		}

		UWidgetComponent* widgetComponent = 
			npcConversationOrder[conversationOrderIndex]->FindComponentByClass<UWidgetComponent>();
		widgetComponent->SetHiddenInGame(false);

		USpeechWidget* speechWidget = Cast<USpeechWidget>(widgetComponent->GetUserWidgetObject());

		speechWidget->dialogueLine = conversationRows[conversationOrderIndex]->dialogueLine;
		speechWidget->speakerName = conversationRows[conversationOrderIndex]->speakerName;

		if (conversationOrderIndex < conversationRows.Num())
		{
			conversationOrderIndex++;
		}
	}
}
