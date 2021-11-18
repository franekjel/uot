#pragma once

#include <memory>
#include <unordered_map>

#include "sector.h"

struct Galaxy
{
    std::unordered_map<int, std::shared_ptr<Sector>> sectors;
};
