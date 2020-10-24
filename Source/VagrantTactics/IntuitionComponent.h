// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Intuition.h"
#include "IntuitionComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VAGRANTTACTICS_API UIntuitionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UIntuitionComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UIntuition> intuitionClass;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//FText uiDisplayText;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//EIntuitionID id;
};
