// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ConversationTable.h"
#include "ConversationInstance.generated.h"

class ANPCUnit;
class UDataTable;
class ABattleInstance;

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

	UPROPERTY(EditAnywhere) TArray<AActor*> npcConversationOrder;

	//Actors to activate based on event or current time of day
	UPROPERTY(EditAnywhere) TArray<AActor*> actorsToActivate;

	TArray<FConversationTable*> conversationRows;

	UPROPERTY(EditAnywhere)
	UDataTable* conversationTable;

	UPROPERTY(EditAnywhere) float timeBetweenTextChanges = 4.f;
	UPROPERTY(EditAnywhere) float initialTimeForConversationStart = 1.0f;

	//Whether to activate instance base on current world time. If 0, always active
	UPROPERTY(EditAnywhere) int hourToActivate = 0;
	UPROPERTY(EditAnywhere) int minuteToActivate = 0;

	int conversationOrderIndex = 0;

	//Whether the player has input in the dialogue (for button presses, etc.)
	UPROPERTY(EditAnywhere) bool bIsPlayerConnected;

	//Whether the instance disregards minutes and acivates on an hourly period instead
	UPROPERTY(EditAnywhere) bool bOnlyActivateOnHour;

	/*If the player starts a battle that needs conversation at the beginning, link it to
	a convsationinstance and then activate the BattleInstance Overlap event once work is done here.*/
	UPROPERTY(EditAnywhere) ABattleInstance* battleInstanceToActivateOnEnd;
};
