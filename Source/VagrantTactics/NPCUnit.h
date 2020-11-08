// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "NPCUnit.generated.h"

class USpeechWidget;
class UDataTable;
class AConversationInstance;

//Just a unit with speech
UCLASS()
class VAGRANTTACTICS_API ANPCUnit : public AUnit
{
	GENERATED_BODY()
public:
	virtual void Tick(float DeltaTime) override;

	//Testing variable to try out movement scripting during dialogue
	UPROPERTY(EditAnywhere) AActor* pointToMoveTo;

	UPROPERTY(BlueprintReadWrite)
	USpeechWidget* speechWidget;

	UPROPERTY(EditAnywhere) UDataTable* speechTable;

	UPROPERTY(EditAnywhere) AConversationInstance* conversationInstance;

	bool bSetToMoveDuringConversation = false;
};
