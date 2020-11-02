#pragma once

#include "SkillInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class USkillInterface : public UInterface
{
    GENERATED_BODY()
};

class AGridActor;
class AActor;
class AUnit;

class ISkillInterface
{
    GENERATED_BODY()

public:
    //Executes skill effect
    virtual void UseSkill(int x, int y, AUnit* owner, AActor* target) = 0;

    //Primes skill and sets up attack nodes (usually called before UseSkill() if a charge skill)
    virtual void ChargeSkill(int x, int y, AUnit* owner, AActor* target) = 0;
};