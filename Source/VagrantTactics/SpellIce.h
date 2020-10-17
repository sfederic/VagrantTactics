// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpellBase.h"
#include "SpellIce.generated.h"

class AActor;

//Spawns ice block in combat
UCLASS()
class VAGRANTTACTICS_API USpellIce : public USpellBase
{
	GENERATED_BODY()
public:
	virtual void CastSpell(int x, int y) override;

	UPROPERTY(EditAnywhere) TSubclassOf<AActor> iceBlockToSpawn;
};
