#pragma once
#include "building.h"
#include "resource.h"

// TODO - where store and how apply bonuses
struct SpaceObjectBonus; // kind of object bonuses

struct SpaceObjectType
{
    std::set<BuildingType> possible_buildings

    SpaceObjectTyp(const std::set<BuildingType> buildings) possible_buildings(buildings)
};

const SpaceObjectType Planet({});
const SpaceObjectType Black_hole({});
const SpaceObjectType Asteroid({});

struct SpaceObject
{
    SpaceObjectType type;
    std::set<Resource> resources;
    std::set<SpaceObjectBonus> bonuses;
    std::set<Building>;
};