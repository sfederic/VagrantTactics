// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SpeechWidget.generated.h"

//Component that hold various speech data for a unit
UCLASS()
class VAGRANTTACTICS_API USpeechWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText speakerName;

	//Normal dialogue line
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText dialogueLine;

	//Line enemy says during combat initiation
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText startCombatLine;
};
