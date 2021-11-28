#pragma once

#include "sector.h"
#include "structs.h"

struct SpaceBase
{
    unsigned int id;
    std::shared_ptr<InhabitableObject> object;
    std::shared_ptr<Player> owner;
};
