#pragma once

#include "sector.h"
#include "structs.h"

struct SpaceBase
{
    std::shared_ptr<InhabitableObject> object;
    std::shared_ptr<Player> owner;
};
