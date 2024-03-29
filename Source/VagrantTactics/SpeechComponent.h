// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpeechComponent.generated.h"

class UWidgetComponent;
class USpeechWidget;
class UDataTable;

//Component that deals with all speech logic 
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VAGRANTTACTICS_API USpeechComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USpeechComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void ShowDialogue(bool bStartCombat);
	void ShowStressDialogue();
	void HideDialogue();

	UPROPERTY(EditAnywhere) UDataTable* conversationTable;

	UPROPERTY(EditAnywhere) FText dialogueText;
	UPROPERTY(EditAnywhere) FText startCombatText;

	//Text shown on unit when stress hits max
	UPROPERTY(EditAnywhere) FText stressText;

	UWidgetComponent* widgetComponent;
	USpeechWidget* speechWidget;
};
