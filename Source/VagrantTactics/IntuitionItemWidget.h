// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Intuition.h"
#include "IntuitionItemWidget.generated.h"

/**
 * 
 */
UCLASS()
class VAGRANTTACTICS_API UIntuitionItemWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite) UIntuition* intuitionRef;
};
