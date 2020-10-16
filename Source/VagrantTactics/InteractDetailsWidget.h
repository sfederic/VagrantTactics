// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractDetailsWidget.generated.h"

//Shows details of an in world item that can be interacted with.
UCLASS()
class VAGRANTTACTICS_API UInteractDetailsWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly) FText detailsText;
};
