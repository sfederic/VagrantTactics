// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "NPCUnit.generated.h"

class USpeechWidget;
class AConversationInstance;
class UWidgetComponent;

//Just a unit with speech
UCLASS()
class VAGRANTTACTICS_API ANPCUnit : public AUnit
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	//Testing variable to try out movement scripting during dialogue
	UPROPERTY(EditAnywhere) AActor* pointToMoveTo;

	//Widgets
	UPROPERTY(BlueprintReadWrite)
	USpeechWidget* speechWidget;

	UWidgetComponent* healthBarWidgetComponent;
	UWidgetComponent* speechWidgetComponent;

	UPROPERTY(EditAnywhere) AConversationInstance* conversationInstance;

	bool bSetToMoveDuringConversation = false;
};
