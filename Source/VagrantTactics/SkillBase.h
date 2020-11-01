// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SkillInterface.h"
#include "SkillBase.generated.h"

//Skills are all physical arts that units can use in battle.
//The main difference between skills and spells can be that skills are charged over one unit turn instead of instant (telegraphing)
UCLASS(Blueprintable, BlueprintType)
class VAGRANTTACTICS_API USkillBase : public UObject, public ISkillInterface
{
	GENERATED_BODY()
public:
	virtual void UseSkill(int x, int y, AUnit* owner, AActor* target) {}

	//To be displayed on UI on skill activation
	UPROPERTY(EditAnywhere) FText skillName;
};
