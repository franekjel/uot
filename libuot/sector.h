#pragma once

#include <map>
#include <memory>
#include <set>

#include "planet.h"
#include "resource.h"
#include "sectorobject.h"

struct Star : SectorObject
{
    enum class StarType  // type affects sector generation
    {
        Sunny,       // increased chances for earth-like planets
        BlueGiant,   // increased chances for hot planets
        RedGiant,    // increased chances for cold planet
        WhiteDwarf,  // colder
        BrownDwarf,  // even colder
        BlackHole,   // no planets, all was sucked in
    };
    StarType star_type;
    Star(const SectorObject &o, const StarType s) : SectorObject(o), star_type(s) {}
};

const std::map<Star::StarType, float> StarTypeToHabitablePlanetChance = {
    {Star::StarType::Sunny, 2.0},      {Star::StarType::BlueGiant, 1.0},   {Star::StarType::RedGiant, 0.6},
    {Star::StarType::WhiteDwarf, 0.3}, {Star::StarType::BrownDwarf, 0.05}, {Star::StarType::BlackHole, 0.0}};

// asteroid, inhabitable planet, gas giants
struct InhabitableObject : SectorObject
{
    std::map<Resource, float> resurce_deposit;  // resource gain after building mine
    enum class ObjectType
    {
        InhabitablePlanet,
        GasGiant,
        Asteroid,
        DarkMatterCloud
    };
    ObjectType object_type;
    InhabitableObject(const SectorObject &o, const std::map<Resource, float> &r, ObjectType t)
        : SectorObject(o), resurce_deposit(r), object_type(t)
    {
    }
};

struct Sector
{
    QPointF position;  // position on galaxy map
    std::set<std::shared_ptr<Sector>> neighbors;
    std::set<std::shared_ptr<SectorObject>> objects;  // stars, planets, asteroid
};
