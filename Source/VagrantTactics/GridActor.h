// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridActor.generated.h"

//Base class for static actors in level.
UCLASS()
class VAGRANTTACTICS_API AGridActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AGridActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere) int xIndex;
	UPROPERTY(VisibleAnywhere) int yIndex;
};
