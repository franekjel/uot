#pragma once
#include <set>
#include "building.h"
#include "resource.h"

// TODO - where store and how apply bonuses
struct SpaceObjectBonus; // kind of object bonuses

struct SpaceObjectType
{
    std::set<BuildingType> possible_buildings;

    SpaceObjectType(const std::set<BuildingType> possible_buildings) : possible_buildings(possible_buildings) {}
};

const SpaceObjectType Planet({});
const SpaceObjectType Black_hole({});
const SpaceObjectType Asteroid({});

struct SpaceObject
{
    SpaceObjectType type;
    std::set<Resource> resources;
    std::set<SpaceObjectBonus> bonuses;
    std::set<Building> buildings;
};