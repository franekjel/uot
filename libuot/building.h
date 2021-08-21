#pragma once
#include <map>
#include "resource.h

//TODO - where store and how apply bonuses
struct BuildingType
{
    std::map<Resource, float> initial_costs;
    int initial_turns_cost;
    int max_level;

    BuildingType(const std::map<Resource, float> costs, int cost, int max)
        : initial_costs(costs), initial_turns_cost(cost), max_level, max
};

struct Headquarter : BuildingType
{
    Headquarter() : ({}, -1, -1);
};

struct Factory : BuildingType
{
    Factory() : ({}, -1, -1);
};

struct MilitaryUnit : BuildingType
{
    MilitaryUnit() : ({}, -1, -1);
};

struct Mine : BuildingType
{
    Resource resource_type;
    Mine() : ({}, -1, -1);
};

struct University : BuildingType
{
    University() : ({}, -1, -1);
};

struct Building
{
    BuildingType type;
    int level;
};