// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ConversationTable.h"
#include "ConversationInstance.generated.h"

class ANPCUnit;
class UDataTable;

//Takes a number of actors and switches between their dialogue through a DataTable
UCLASS()
class VAGRANTTACTICS_API AConversationInstance : public AActor
{
	GENERATED_BODY()
	
public:	
	AConversationInstance();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	void ShowNextDialogueLineOnTimer();
	void ShowNextDialogueLineOnPlayerInput();

	UPROPERTY(EditAnywhere)
	TArray<AActor*> npcConversationOrder;

	TArray<FConversationTable*> conversationRows;

	UPROPERTY(EditAnywhere)
	UDataTable* conversationTable;

	UPROPERTY(EditAnywhere) float timeBetweenTextChanges = 4.f;
	UPROPERTY(EditAnywhere) float initialTimeForConversationStart = 1.0f;

	int conversationOrderIndex = 0;

	//Whether the player has input in the dialogue (for button presses, etc.)
	UPROPERTY(EditAnywhere) bool bIsPlayerConnected;
};
