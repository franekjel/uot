#pragma once

#include <memory>
#include <set>

#include "sector.h"

struct Galaxy
{
    std::set<std::shared_ptr<Sector>> sectors;
};
