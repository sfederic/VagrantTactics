// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillBase.h"
#include "SkillLineAttack.generated.h"

//Skill that targets along a straight line from owner. No Target.
UCLASS()
class VAGRANTTACTICS_API USkillLineAttack : public USkillBase
{
	GENERATED_BODY()
public:
	virtual void UseSkill(int x, int y, AUnit* owner, AGridActor* target);
};
