// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EventInstance.generated.h"

//Instance to activate whatever (actors, other instances) based on certain parameters
UCLASS()
class VAGRANTTACTICS_API AEventInstance : public AActor
{
	GENERATED_BODY()
	
public:	
	AEventInstance();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere) TArray<AActor*> actorsToActivate;

	//If 0, always active;
	UPROPERTY(EditAnywhere) int timeOfDayActivation = 0;

	//A generic bool that just serves a link
	UPROPERTY(EditAnywhere) bool bActivate;
};
