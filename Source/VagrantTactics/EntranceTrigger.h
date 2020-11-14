// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EntranceKeys.h"
#include "EntranceTrigger.generated.h"

class UBoxComponent;

//Trigger to doors in level between rooms.
UCLASS()
class VAGRANTTACTICS_API AEntranceTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	AEntranceTrigger();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UBoxComponent* box;

	UPROPERTY(EditAnywhere) FName connectedLevel;

	//Key to entrace. Held in GameInstance
	UPROPERTY(EditAnywhere) FName entranceKey;
};
