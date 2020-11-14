// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractTrigger.generated.h"

class AGridActor;

//Base class for interaction boxes. 
UCLASS()
class VAGRANTTACTICS_API AInteractTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	AInteractTrigger();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	class UBoxComponent* box;

	//Initial interact prompt
	UPROPERTY(EditAnywhere) FText interactText;

	//Details once player interacts with object
	UPROPERTY(EditAnywhere) FText detailsText;

	UPROPERTY(EditAnywhere) FName keyToPickup;

	//Connected actors to mark for Destroy()/Change material on player interaction or focus on
	UPROPERTY(EditAnywhere) AActor* connectedActor;

	//Denotes whether the connectedActor can be picked up and added to player mesh socket
	UPROPERTY(EditAnywhere) bool bPickupConnectedActor;
};
