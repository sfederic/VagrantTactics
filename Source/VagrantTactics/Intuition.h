// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "IntuitionInterface.h"
#include "Intuition.generated.h"

UENUM(BlueprintType)
enum class EIntuitionID : uint8
{
	NonStoryEffect
};

//Event based buffs the player collects in-game
UCLASS(BlueprintType, Blueprintable)
class VAGRANTTACTICS_API UIntuition : public UObject, public IIntuitionInterface
{
	GENERATED_BODY()
public:
	virtual void AddIntuition() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText uiDisplayText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EIntuitionID id;
};
