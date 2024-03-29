// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IntuitionDialogue.h"
#include "BattleInstance.generated.h"

class AUnit;
class UBoxComponent;
class AConversationInstance;
class UIntuition;

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

	UPROPERTY(VisibleAnywhere) TArray<AUnit*> unitsToActivateOnBattleStart;

	UPROPERTY(EditAnywhere)
	TArray<FIntuitionDialogue> speechOnBattleEnd;

	UPROPERTY(EditAnywhere) TSubclassOf<UIntuition> intuitionToGainOnBattleEnd;

	UPROPERTY(EditAnywhere) AConversationInstance* conversationInstanceToActivateOnOverlap;

	UPROPERTY(VisibleAnywhere) int numOfUnitsAlive;

	UBoxComponent* box;
};
