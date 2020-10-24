#pragma once

#include "IntuitionInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UIntuitionInterface : public UInterface
{
    GENERATED_BODY()
};

class IIntuitionInterface
{
    GENERATED_BODY()

public:
    virtual void AddIntuition() = 0;
};