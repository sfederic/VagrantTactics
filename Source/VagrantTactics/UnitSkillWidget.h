// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UnitSkillWidget.generated.h"

//Widget to display simple information on castin unit skill/spell
UCLASS()
class VAGRANTTACTICS_API UUnitSkillWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FText skillNameToDisplay;
};
