// Fill out your copyright notice in the Description page of Project Settings.

#include "ConversationInstance.h"
#include "NPCUnit.h"
#include "TimerManager.h"
#include "ConversationTable.h"
#include "Components/WidgetComponent.h"
#include "SpeechWidget.h"
#include "PlayerUnit.h"
#include "Kismet/GameplayStatics.h"
#include "GameplayTags.h"
#include "MainGameInstance.h"

AConversationInstance::AConversationInstance()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AConversationInstance::BeginPlay()
{
	Super::BeginPlay();
	
	UMainGameInstance* gameInstance = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (timeOfDayToActivate != 0)
	{
		if (timeOfDayToActivate != gameInstance->currentTimeOfDay)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s not set to activate at %d time of day."), *GetName(), gameInstance->currentTimeOfDay);

			//Hide actors in scene
			for (AActor* actor : actorsToActivate)
			{
				actor->SetActorHiddenInGame(true);
			}

			Destroy();
		}
		else
		{
			//Activate actors in scene
			for (AActor* actor : actorsToActivate)
			{
				actor->SetActorHiddenInGame(false);
			}
		}
	}

	if (conversationTable)
	{
		FString contextString;
		conversationTable->GetAllRows(contextString, conversationRows);
	}

	if (!bIsPlayerConnected)
	{
		FTimerHandle handle;
		GetWorldTimerManager().SetTimer(handle, this, &AConversationInstance::ShowNextDialogueLineOnTimer, initialTimeForConversationStart, false);
	}
}

void AConversationInstance::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AConversationInstance::ShowNextDialogueLineOnTimer()
{
	//TArray<UActorComponent*> speechWidgetComponents = npcConversationOrder[conversationOrderIndex]->GetComponentsByTag(
	//	UWidgetComponent::StaticClass(), ComponentTags::HealthBar);

	if (conversationOrderIndex >= conversationRows.Num())
	{
		if (conversationOrderIndex > 0)
		{
			UWidgetComponent* speechWidgetComponent = Cast<UWidgetComponent>(npcConversationOrder[conversationOrderIndex - 1]->GetDefaultSubobjectByName(TEXT("WidgetSpeech")));
			speechWidgetComponent->SetHiddenInGame(false);
		}

		UE_LOG(LogTemp, Warning, TEXT("%s conversation finished."), *this->GetName());

		//Testing case for moving NPCs at end of conversation
		for (AActor* actor : npcConversationOrder)
		{
			ANPCUnit* npc = Cast<ANPCUnit>(actor);
			if (npc)
			{
				npc->bSetToMoveDuringConversation = true;
			}
		}

		Destroy();
		return;
	}

	FTimerHandle handle;
	GetWorldTimerManager().SetTimer(handle, this, &AConversationInstance::ShowNextDialogueLineOnTimer, timeBetweenTextChanges, false);

	if (npcConversationOrder.Num() > 0)
	{
		//Don't hide previous speech widget if not first 
		if (conversationOrderIndex > 0)
		{
			UWidgetComponent* speechWidgetComponent = 
				Cast<UWidgetComponent>(npcConversationOrder[conversationOrderIndex - 1]->GetDefaultSubobjectByName(TEXT("WidgetSpeech")));
			speechWidgetComponent->SetHiddenInGame(true);
		}

		UWidgetComponent* speechWidgetComponent = 
			Cast<UWidgetComponent>(npcConversationOrder[conversationOrderIndex]->GetDefaultSubobjectByName(TEXT("WidgetSpeech")));
		speechWidgetComponent->SetHiddenInGame(false);

		USpeechWidget* speechWidget = Cast<USpeechWidget>(speechWidgetComponent->GetUserWidgetObject());
		speechWidget->dialogueLine = conversationRows[conversationOrderIndex]->dialogueLine;
		speechWidget->speakerName = conversationRows[conversationOrderIndex]->speakerName;

		if (conversationOrderIndex < conversationRows.Num())
		{
			conversationOrderIndex++;
		}
	}
}

void AConversationInstance::ShowNextDialogueLineOnPlayerInput()
{
	if (conversationOrderIndex >= conversationRows.Num())
	{
		if (conversationOrderIndex > 0)
		{
			UWidgetComponent* speechWidgetComponent =
				Cast<UWidgetComponent>(npcConversationOrder[conversationOrderIndex - 1]->GetDefaultSubobjectByName(TEXT("WidgetSpeech")));
			speechWidgetComponent->SetHiddenInGame(true);
		}

		UE_LOG(LogTemp, Warning, TEXT("%s conversation finished."), *this->GetName());

		//Testing case for moving NPCs at end of conversation
		for (AActor* actor : npcConversationOrder)
		{
			ANPCUnit* npc = Cast<ANPCUnit>(actor);
			if (npc)
			{
				npc->bSetToMoveDuringConversation = true;
			}
		}

		APlayerUnit* player = Cast<APlayerUnit>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		player->connectedConversationInstance = nullptr;
		player->bInConversation = false;
		player->ResetCameraFocusAndFOV();

		conversationOrderIndex = 0;

		return;
	}


	if (npcConversationOrder.Num() > 0)
	{
		//Don't hide previous speech widget if not first 
		if (conversationOrderIndex > 0)
		{
			UWidgetComponent* speechWidgetComponent =
				Cast<UWidgetComponent>(npcConversationOrder[conversationOrderIndex - 1]->GetDefaultSubobjectByName(TEXT("WidgetSpeech")));
			speechWidgetComponent->SetHiddenInGame(true);
		}

		UWidgetComponent* speechWidgetComponent =
			Cast<UWidgetComponent>(npcConversationOrder[conversationOrderIndex]->GetDefaultSubobjectByName(TEXT("WidgetSpeech")));
		speechWidgetComponent->SetHiddenInGame(false);

		//Set player camera focus on current actor speaking
		APlayerUnit* player = Cast<APlayerUnit>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		player->conversationWidgetFocus = speechWidgetComponent->GetComponentLocation();

		USpeechWidget* speechWidget = Cast<USpeechWidget>(speechWidgetComponent->GetUserWidgetObject());

		speechWidget->dialogueLine = conversationRows[conversationOrderIndex]->dialogueLine;
		speechWidget->speakerName = conversationRows[conversationOrderIndex]->speakerName;

		if (conversationOrderIndex < conversationRows.Num())
		{
			conversationOrderIndex++;
		}
	}
}
