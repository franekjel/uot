#pragma once

#include <algorithm>

#include "common.h"

// base properties of every object in sector
struct SectorObject
{
    unsigned int id;
    Point position;  // position in sector - (0,0) is sector center
    float size;      // size multipler agains base size (from model)
    unsigned int sector_id;
    SectorObject() = default;
    SectorObject(const unsigned int id, Point _position, float _size, unsigned int _sector_id)
        : id(id), position(_position), size(_size), sector_id(_sector_id){};

    virtual ~SectorObject() = default;
};
