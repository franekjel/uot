#pragma once

#include <algorithm>

#include "common.h"

// base properties of every object in sector
struct SectorObject
{
    Point position;  // position in sector - (0,0) is sector center
    float size;      // size multipler agains base size (from model)
};
