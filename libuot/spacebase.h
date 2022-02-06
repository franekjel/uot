#pragma once

#include "sector.h"
#include "structs.h"

struct SpaceBase
{
    unsigned int id;
    std::shared_ptr<InhabitableObject> object;
    std::shared_ptr<Player> owner;

    SpaceBase() = default;
    SpaceBase(unsigned int id, const std::shared_ptr<InhabitableObject>& object, const std::shared_ptr<Player>& owner)
        : id(id), object(object), owner(owner)
    {
    }
};
