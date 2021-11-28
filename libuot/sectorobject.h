#pragma once

#include <algorithm>

#include "common.h"

// base properties of every object in sector
struct SectorObject
{
    unsigned int id;
    Point position;  // position in sector - (0,0) is sector center
    float size;      // size multipler agains base size (from model)
    SectorObject() = default;
    SectorObject(const unsigned int id, Point _position, float _size) : id(id), position(_position), size(_size){};

    virtual ~SectorObject() = default;
};
