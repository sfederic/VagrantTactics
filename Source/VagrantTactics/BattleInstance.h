// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BattleInstance.generated.h"

class AUnit;
class UBoxComponent;

//The instance that connects all units in an area and instigates battles based on area.
UCLASS()
class VAGRANTTACTICS_API ABattleInstance : public AActor
{
	GENERATED_BODY()
	
public:	
	ABattleInstance();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION() 
	void ActivateBattleOnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	UPROPERTY(EditAnywhere) TArray<AUnit*> unitsToActivateOnBattleStart;

	UBoxComponent* box;
};
