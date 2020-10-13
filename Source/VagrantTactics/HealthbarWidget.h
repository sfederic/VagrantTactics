// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthbarWidget.generated.h"

class AGridActor;

//Unit health bar in screen space
UCLASS()
class VAGRANTTACTICS_API UHealthbarWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly) AGridActor* attachedUnit;
};
