#pragma once

#include "SpellInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class USpellInterface : public UInterface
{
    GENERATED_BODY()
};

class AGridActor;

class ISpellInterface
{
    GENERATED_BODY()

public:
    //The arguments are for the grid node index. 
    virtual void CastSpell(int x, int y, AGridActor* target) = 0;
};