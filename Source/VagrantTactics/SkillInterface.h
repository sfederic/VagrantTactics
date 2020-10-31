#pragma once

#include "SkillInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class USkillInterface : public UInterface
{
    GENERATED_BODY()
};

class AGridActor;
class AUnit;

class ISkillInterface
{
    GENERATED_BODY()

public:
    //arguments are for the grid node index. 
    virtual void UseSkill(int x, int y, AUnit* owner, AGridActor* target) = 0;
};