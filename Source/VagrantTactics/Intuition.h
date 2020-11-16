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

	//text to show when hovering over the widget item
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText tooltipText;

	//What to show in UI when browsing intuitions
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText uiDisplayText;

	//Just a string to ID the intuition for checks
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName intuitonID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* tooltipImage;
};
