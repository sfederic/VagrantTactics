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
	void ShowNextDialogueLine();

	UPROPERTY(EditAnywhere)
	TArray<ANPCUnit*> npcConversationOrder;

	TArray<FConversationTable*> conversationRows;

	UPROPERTY(EditAnywhere)
	UDataTable* conversationTable;

	int conversationOrderIndex = 0;
};
