// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupItem.generated.h"

class UBoxComponent;

//Item that units and player can pickup and use as active inventory
UCLASS()
class VAGRANTTACTICS_API APickupItem : public AActor
{
	GENERATED_BODY()
	
public:	
	APickupItem();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//Text to display on player's Interact Widget
	UPROPERTY(EditAnywhere) FText interactText;

	UBoxComponent* box;

	UPROPERTY(EditAnywhere) bool bDisableBoxComponentOnSpawn;
};
