// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SpeechWidget.generated.h"

/**
 * 
 */
UCLASS()
class VAGRANTTACTICS_API USpeechWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText speakerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText dialogueLine;
};
