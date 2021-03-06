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

const std ::map<Star::StarType, std::pair<std::string, std::string>> StarToNameDesc = {
    {Star::StarType::Sunny, {"Solar Twin", "A star, very similar to our good old Sun"}},
    {Star::StarType::BlueGiant, {"Blue Giant", "Huge, extremely hot and luminous, burn helium"}},
    {Star::StarType::RedGiant,
     {"Red Giant", "Has relatively low temperature, but remains pretty luminous due to enormous size"}},
    {Star::StarType::WhiteDwarf, {"White Dwarf", "Small, faint, very dense, hot star near the end of its life"}},
    {Star::StarType::BrownDwarf, {"Brown Dwarf", "Something in between a star and a gas giant"}},
    {Star::StarType::BlackHole, {"Black Hole", "So dense, even light can't escape its gravitational field"}}};

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
        float work = 40.0f;
        cost[Resource::Metals] = 100;
        Resource deposit_type = resource_deposit.begin()->first;
        switch (deposit_type)
        {
            case Resource::RareMetals:
                cost[Resource::Metals] += 100;
                work += 50.0f;
                break;
            case Resource::Crystals:
                cost[Resource::Metals] += 100;
                work += 50.0f;
                break;
            case Resource::Polymers:
                cost[Resource::Crystals] = 40;
                cost[Resource::RareMetals] = 40;
                work += 60.0f;
                break;
            default:
                break;
        }

        return {work, cost};
    }
};

const std ::map<InhabitableObject::ObjectType, std::pair<std::string, std::string>> InhToNameDesc = {
    {InhabitableObject::ObjectType::Asteroid, {"Asteroid", "Rocky object, much smaller than a planet"}},
    {InhabitableObject::ObjectType::DarkMatterCloud,
     {"Dark Mater Cloud", "Cloud of ultralight, almost impossible to detect, particles"}},
    {InhabitableObject::ObjectType::InhabitablePlanet,
     {"Uninhabitable Planet", "Solid planet, but not suitable for colony"}},
    {InhabitableObject::ObjectType::GasGiant, {"Gas Giant", "This planet has no hard surface"}},

};

struct Sector
{
    /* Sector is circle with R=1.0 and star in centre
     */

    unsigned int sector_id;
    Point position;  // position on galaxy map
    std::set<std::shared_ptr<Sector>> neighbors;
    std::map<unsigned int, std::shared_ptr<SectorObject>> objects;
    std::map<unsigned int, std::shared_ptr<Fleet>> present_fleets;

    Sector() = default;
    Sector(const unsigned int sector_id, const Point &position,
           const std::map<unsigned int, std::shared_ptr<SectorObject>> &objects = {})
        : sector_id(sector_id), position(position), objects(objects)
    {
    }

    // fields below are server-only
    std::map<unsigned int, unsigned int> watchers;  // players who are watching the sector, only for server purpose
    std::map<unsigned int, int> new_watchers;

    struct FleetParameters
    {
        unsigned int fleet_id;
        Point position;
        float human_capacity;
        float construction_points;
        float base_fleet_speed;
        float current_hp;
        float max_hp;
        float current_shields;
        float max_shields;
        float average_energy;

        FleetParameters() = default;
        FleetParameters(std::shared_ptr<Fleet> fleet);
    };

    struct JoinedFleets
    {
        unsigned int joined_fleet_1;
        unsigned int joined_fleet_2;
        unsigned int owner;
        FleetParameters fleet_parameters;
    };

    struct JumpedFleet
    {
        unsigned int fleet_id;
        unsigned int sector_id_from;
        unsigned int sector_id_to;
        Point position;
        unsigned int owner;
    };

    struct NewBase
    {
        unsigned int base_id;
        unsigned int object_id;
        unsigned int owner;
    };

    struct NewColony
    {
        unsigned int colony_id;
        unsigned int object_id;
        unsigned int owner;
        float population;
    };

    struct DestroyedShip
    {
        unsigned int ship_id;
        unsigned int owner;
    };

    struct FleetPopChange
    {
        unsigned int fleet_id;
        unsigned int owner;
        float civilians;
        float soldiers;
    };

    void DecrementWatcher(unsigned int player_id);
    void IncrementWatcher(unsigned int player_id);
    void JumpFleet(unsigned int fleet_id);
    std::vector<JoinedFleets> joined_fleets;
    std::vector<JumpedFleet> jumped_fleets;
    std::vector<NewBase> new_bases;
    std::vector<NewColony> new_colonies;
    std::vector<std::shared_ptr<Fleet>> fleets_in_fight;
    std::vector<DestroyedShip> destroyed_ships;
    std::vector<FleetPopChange> fleets_changed;
};
