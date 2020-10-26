// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "NPCUnit.generated.h"

class USpeechWidget;

//Just a unit with speech
UCLASS()
class VAGRANTTACTICS_API ANPCUnit : public AUnit
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite)
	USpeechWidget* speechWidget;
};
