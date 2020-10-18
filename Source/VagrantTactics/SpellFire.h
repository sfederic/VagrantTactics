// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpellBase.h"
#include "SpellFire.generated.h"

class UNiagaraSystem;

//Fireball spell on single target
UCLASS()
class VAGRANTTACTICS_API USpellFire : public USpellBase
{
	GENERATED_BODY()
public:
	virtual void CastSpell(int x, int y, AGridActor* target) override;

	UPROPERTY(EditAnywhere) UNiagaraSystem* particleFire;
};
