// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IntuitionsWidget.generated.h"

//Single item for populating List of intuitions widget
UCLASS()
class VAGRANTTACTICS_API UIntuitionsWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	//The actual text shown in UI
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText displayText;
};
