// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SpellInterface.h"
#include "SpellBase.generated.h"

//Base class for spells.
UCLASS(Blueprintable, BlueprintType)
class VAGRANTTACTICS_API USpellBase : public UObject, public ISpellInterface
{
	GENERATED_BODY()
public:
	virtual void CastSpell(int x, int y, AGridActor* target) {};
};
