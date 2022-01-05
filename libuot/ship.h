#pragma once

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "modules.h"
#include "resource.h"
#include "sector.h"

struct ShipHull
{
    int sides_size;
    int back_size;
    int inside_size;
    std::map<Resource, float> cost;
    std::map<Resource, float> additional_upkeep;
    float hp;
    float speed;
    float engines_energy_consumtion;
    float crew;  // people needed to crew
    float worker_weeks_cost;

    ShipHull(const int sides_size, const int inside_size, const std::map<Resource, float>& cost,
             const std::map<Resource, float>& additional_upkeep, const float hp, const float engines_power,
             const float engines_energy_consumtion, const float crew, const float worker_weeks_cost);
};

const ShipHull SmallShipHull(4, 4, {{Resource::Metals, 50.0f}}, {{Resource::Antimatter, 0.5f}}, 80.0f, 0.1f, 1.0f, 0.1f,
                             60.0f);
const ShipHull MediumShipHull(12, 12, {{Resource::Metals, 100.0f}, {Resource::RareMetals, 20.0f}},
                              {{Resource::Antimatter, 1.2f}}, 250.0f, 0.08f, 2.5f, 0.25f, 120.0f);
const ShipHull GrandShipHull(36, 36,
                             {{Resource::Metals, 200.0f}, {Resource::RareMetals, 40.0f}, {Resource::Polymers, 20.0f}},
                             {{Resource::Antimatter, 2.5f}}, 700.0f, 0.07f, 4.0f, 0.5f, 250.0f);

struct ShipDesign
{
    unsigned int id;
    std::string name;
    const ShipHull* hull;
    std::map<ModuleType, int> sides;
    std::map<ModuleType, int> inside;
    std::map<Resource, float> cost;    // sum of costs of modules and hull
    std::map<Resource, float> upkeep;  // total upkeep
    float worker_weeks_cost;
    static constexpr float percentage_cost_upkeep = 0.05f;  // which part of ship cost goes to weekly upkeep

    ShipDesign(const unsigned int id, const std::string& name, const ShipHull* hull,
               const std::map<ModuleType, int>& sides, const std::map<ModuleType, int>& inside);
};

struct Ship
{
    unsigned int id;
    float hp;
    float max_hp;
    float hp_regen;
    float shield;
    float max_shield;
    static constexpr float shield_regen_percentage = 0.03f;  // shield regenerating ratio
    static constexpr float shield_regen_energy_cost = 0.1f;  // energy cost per shield point
    static constexpr float shield_regen_energy_cost_reverse =
        1.0f / shield_regen_energy_cost;  // energy cost per shield point but reverse;
    float energy;
    float energy_regen;
    float max_energy;
    float soldiers;
    float civilians;
    float human_capacity;
    float speed;
    float engines_energy_consumtion;
    float construction_points;
    std::vector<Weapon> weapons;
    std::shared_ptr<ShipDesign> design;

    std::shared_ptr<Ship> ShipFromDesign(const int id, const std::shared_ptr<ShipDesign> design);

    float GetShipSpeed();

    void RegenShip();

    void MoveShip(float distance) { energy -= engines_energy_consumtion * (distance / speed); }
};

struct Fleet
{
    unsigned int id;
    std::vector<std::shared_ptr<Ship>> ships;
    float soldiers;
    float civilians;
    float human_capacity;
    float construction_points;
    std::shared_ptr<Sector> location_sector;
    Point position;
    Point wanted_position = {NAN, NAN};
    float fleet_speed_per_turn = -1.0f;
    bool empty_fleet = false;  // if true then fleet needs to be deleted
    unsigned int owner_id;

    void UpdateFleet();

    void UpdateFleetSpeed();

    void MoveFleet();
};
