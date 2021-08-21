#pragma once
#include <map>
#include "resource.h"

// TODO - where store and how apply bonuses
struct BuildingType
{
    std::map<Resource, float> initial_resources_costs;
    int initial_turns_cost;
    int max_level;

    BuildingType(const std::map<Resource, float> initial_resources_costs, int initial_turns_cost, int max_level)
        : initial_resources_costs(initial_resources_costs), initial_turns_cost(initial_turns_cost), max_level(max_level)
    {
    }
};

struct Headquarter : BuildingType
{
    Headquarter() : BuildingType({}, -1, -1){};
};

struct Factory : BuildingType
{
    Factory() : BuildingType({}, -1, -1){};
};

struct MilitaryUnit : BuildingType
{
    MilitaryUnit() : BuildingType({}, -1, -1){};
};

struct Mine : BuildingType
{
    Resource resource_type;
    Mine() : BuildingType({}, -1, -1){};
};

struct University : BuildingType
{
    University() : BuildingType({}, -1, -1){};
};

struct Building
{
    BuildingType type;
    int level;
};