#pragma once

#include <map>
#include <memory>
#include <set>

#include "common.h"
#include "planet.h"
#include "resource.h"
#include "sectorobject.h"
#include "structs.h"

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
    std::map<Resource, float> resource_deposit;
    std::shared_ptr<SpaceBase> base;
    enum class ObjectType
    {
        InhabitablePlanet,
        GasGiant,
        Asteroid,
        DarkMatterCloud
    };
    ObjectType object_type;
    InhabitableObject(const SectorObject &o, const std::map<Resource, float> &r, ObjectType t)
        : SectorObject(o), resource_deposit(r), object_type(t)
    {
    }

    std::pair<float, std::map<Resource, float>> CalcBaseCost()
    {
        if (resource_deposit.empty())
            return {0, {}};
        std::map<Resource, float> cost;
        float work = 100.0f;
        cost[Resource::Metals] = 100;
        Resource deposit_type = resource_deposit.begin()->first;
        switch (deposit_type)
        {
            case Resource::RareMetals:
                cost[Resource::Metals] += 100;
                work += 200.0f;
                break;
            case Resource::Crystals:
                cost[Resource::Metals] += 100;
                work += 200.0f;
                break;
            case Resource::Polymers:
                cost[Resource::Crystals] = 40;
                cost[Resource::RareMetals] = 40;
                work += 300.0f;
                break;
        }

        return {work, cost};
    }
};

struct Sector
{
    /* Sector is circle with R=1.0 and star in centre
     */

    int sector_id;
    Point position;  // position on galaxy map
    std::set<std::shared_ptr<Sector>> neighbors;
    std::map<unsigned int, std::shared_ptr<SectorObject>> objects;
    std::map<unsigned int, std::shared_ptr<Fleet>> present_fleets;

    // fields below are server-only
    std::map<unsigned int, unsigned int> watchers;  // players who are watching the sector, only for server purpose
    std::map<unsigned int, int> new_watchers;
    struct JoinedFleets
    {
        unsigned int joined_fleet_1;
        unsigned int joined_fleet_2;
        unsigned int res_fleet;
        Point res_fleet_pos;
    };

    void DecrementWatcher(unsigned int player_id);
    void IncrementWatcher(unsigned int player_id);
    void JumpFleet(unsigned int fleet_id);
    std::vector<JoinedFleets> joined_fleets;
};
