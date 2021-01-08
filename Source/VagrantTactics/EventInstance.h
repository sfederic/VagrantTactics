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
	void ActivateLinkedActors();
	void DeactivateLinkedActors();

	UPROPERTY(EditAnywhere) TArray<AActor*> actorsToActivate;

	//If 0, always active;
	UPROPERTY(EditAnywhere) int hourToActivate = 0;
	UPROPERTY(EditAnywhere) int minuteToActivate = 0;

	//Diregards minutes and just activates instance over an hour (7 to 8 for example)
	UPROPERTY(EditAnywhere) bool bActiveOverEntireHour = true;
};
