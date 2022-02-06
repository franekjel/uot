#pragma once

#include <memory>
#include <set>

#include "sector.h"

struct Galaxy
{
    std::map<unsigned int, std::shared_ptr<Sector>> sectors;
};
