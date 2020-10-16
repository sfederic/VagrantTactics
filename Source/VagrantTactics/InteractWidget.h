// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractWidget.generated.h"

//Widget for displaying interact UI over player head (opening doors, reading notes, pickup items, etc.)
UCLASS()
class VAGRANTTACTICS_API UInteractWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly) FText interactText;
};
