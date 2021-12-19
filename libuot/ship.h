#pragma once

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "resource.h"
#include "sector.h"

// TODO These modules (like SmallReactor etc.) should be hold in some global set when we will be doing usable version
struct Module
{
    std::string name;
    int size;
    float energy_usage;
    std::map<Resource, float> cost;
    std::map<Resource, float> additional_upkeep;  // base upkeep of module is percentage of it's costs (amortization!)

    enum ModuleDestination
    {
        None = 0,
        Front = 1,
        Sides = 2,
        Back = 4,
        Inside = 8,
        All = 15,
    };
    int destination;

    Module(const std::string& name, const int size, const float energy_usage, const std::map<Resource, float>& cost,
           const std::map<Resource, float>& additional_upkeep, const int destination)
        : name(name),
          size(size),
          energy_usage(energy_usage),
          cost(cost),
          additional_upkeep(additional_upkeep),
          destination(destination)
    {
    }
};

struct Reactor : Module
{
    float generating_energy;

    Reactor(const Module& m, const float generating_energy) : Module(m), generating_energy(generating_energy) {}
};

const Reactor SmallReactor(Module("Small reactor", 2, 0.0f, {{Resource::Metals, 5.0f}}, {{Resource::Antimatter, 1.0f}},
                                  Module::Inside),
                           10.0f);
const Reactor MediumReactor(Module("Medium reactor", 6, 0.0f, {{Resource::Metals, 20.0f}},
                                   {{Resource::Antimatter, 4.0f}}, Module::Inside),
                            40.0f);
const Reactor ImprovedSmallReactor(Module("Improved small reactor", 2, 0.0f, {{Resource::Metals, 6.0f}},
                                          {{Resource::Antimatter, 1.2f}}, Module::Inside),
                                   15.0f);

struct ShieldGenerator : Module
{
    float shield_capacity;
    ShieldGenerator(const Module& m, const float shield_capacity) : Module(m), shield_capacity(shield_capacity) {}
};
const ShieldGenerator SmallShieldGenerator(Module("Small shield generator", 2, 4.0f,
                                                  {{Resource::Metals, 10.0f}, {Resource::Crystals, 5.0f}}, {},
                                                  Module::Inside),
                                           100.0f);

struct ArmorPlates : Module
{
    float additional_hp;
    ArmorPlates(const Module& m, const float additional_hp) : Module(m), additional_hp(additional_hp) {}
};
const ArmorPlates SmallArmorPlates(Module("Small armor plates", 2, 0.0f, {{Resource::Metals, 15.0f}}, {},
                                          Module::Front | Module::Sides | Module::Back),
                                   100.0f);
const ArmorPlates ImprovedSmallArmorPlates(Module("Small armor plates", 2, 0.0f,
                                                  {{Resource::Metals, 15.0f}, {Resource::RareMetals, 5.0f}}, {},
                                                  Module::Front | Module::Sides | Module::Back),
                                           100.0f);

struct Engines : Module
{
    float power;
    Engines(const Module& m, const float power) : Module(m), power(power) {}
};
const Engines SmallEngines(Module("Small engines", 2, 2.0f, {{Resource::Metals, 5.0f}}, {{Resource::Antimatter, 1.0f}},
                                  Module::Back),
                           100.0f);

struct Quarters : Module
{
    static constexpr float capacity = 10;
    Quarters()
        : Module("Cabins", 4, 1.0f, {{Resource::Metals, 25.0f}}, {{Resource::Cryptocurrencies, 2.0f}}, Module::All)
    {
    }
} const Quarters;  // technology cannot make more people live on the same space so there is only one Quarters object

struct ContructionModule : Module
{
    float contruction_speed;
    ContructionModule(const Module& m, float contruction_speed) : Module(m), contruction_speed(contruction_speed){};
};

const ContructionModule BasicContructionModule(Module("Basic construction module", 8, 4.0f, {{Resource::Metals, 10.0f}},
                                                      {}, Module::Front || Module::Sides),
                                               1.0f);
const ContructionModule AdvancedContructionModule(
    Module("Advanced construction module", 8, 4.0f,
           {{Resource::Metals, 12.0f}, {Resource::RareMetals, 5.0f}, {Resource::Crystals, 2.0f}}, {},
           Module::Front || Module::Sides),
    3.0f);

const ContructionModule NanobotsContructionModule(
    Module("Nanobots construction module", 8, 4.0f,
           {{Resource::Metals, 10.0f}, {Resource::RareMetals, 4.0f}, {Resource::AncientNanobots, 4.0f}}, {},
           Module::All),
    8.0f);

struct Weapon
{
    int attack_count;
    float damage;
    float accurracy;
    float range;

    enum SpecialFeatures
    {
        None = 0,
        BypassShield = 1,
        ShieldDamageBonus = 2,
        ArmorDamageBonus = 4,
    };
    int special_features;

    Weapon(const int attack_count, const float damage, const float accurracy, const float range,
           const int special_features)
        : attack_count(attack_count),
          damage(damage),
          accurracy(accurracy),
          range(range),
          special_features(special_features)
    {
    }
};

struct WeaponModule : Module, Weapon
{
    WeaponModule(const Module& m, const Weapon& w) : Module(m), Weapon(w) {}
};

const WeaponModule SmallLaser1Module(Module("Small laser", 1, 2.0f, {{Resource::Metals, 5.0f}}, {},
                                            Module::Front | Module::Sides | Module::Back),
                                     Weapon(5, 4.0f, 0.8f, 20.0f, Weapon::ArmorDamageBonus));
const WeaponModule SmallLaser2Module(Module("Improved small laser", 1, 2.0f,
                                            {{Resource::Metals, 5.0f}, {Resource::Crystals, 3.0f}}, {},
                                            Module::Front | Module::Sides | Module::Back),
                                     Weapon(6, 6.0f, 0.85f, 25.0f, Weapon::ArmorDamageBonus));

// TODO later: special modules like hp regeneration -

struct ShipHull
{
    int front_size;
    int sides_size;
    int back_size;
    int inside_size;
    std::map<Resource, float> cost;
    std::map<Resource, float> additional_upkeep;

    ShipHull(const int front_size, const int sides_size, const int back_size, const int inside_size,
             const std::map<Resource, float>& cost, const std::map<Resource, float>& additional_upkeep)
        : front_size(front_size),
          sides_size(sides_size),
          back_size(back_size),
          inside_size(inside_size),
          cost(cost),
          additional_upkeep(additional_upkeep)
    {
    }
};

const ShipHull SmallShipHull(2, 4, 1, 4, {{Resource::Metals, 25.0f}}, {{Resource::Cryptocurrencies, 10.0f}});
const ShipHull MediumShipHull(6, 12, 4, 10, {{Resource::Metals, 80.0f}}, {{Resource::Cryptocurrencies, 30.0f}});

struct ShipDesign
{
    unsigned int id;
    std::string name;
    ShipHull hull;
    std::set<Module> front;
    std::set<Module> sides;
    std::set<Module> back;
    std::set<Module> inside;
    std::map<Resource, float> cost;    // sum of costs of modules and hull
    std::map<Resource, float> upkeep;  // total upkeep
    float ship_size = 0.0f;

    static constexpr float percentage_cost_upkeep = 0.01;  // which part of ship cost goes to (monthly?) upkeep

    ShipDesign(const unsigned int id, const std::string& name, const ShipHull& hull, const std::set<Module>& front,
               const std::set<Module>& sides, const std::set<Module>& back, const std::set<Module>& inside);
};

struct Ship
{
    unsigned int id;
    float hp;
    float max_hp;
    float hp_regen;
    float shield;
    float max_shield;
    static constexpr float shield_regen_percentage = 0.01;  // shield regenerating ratio
    static constexpr float shield_regen_energy_cost = 0.1;  // energy cost per shield point
    static constexpr float shield_regen_energy_cost_reverse =
        1.0 / shield_regen_energy_cost;  // energy cost per shield point but reverse;
    float energy;
    float energy_regen;
    float max_energy;
    float soldiers;
    float civilians;
    float human_capacity;
    float engines_power;
    float engines_energy_consumtion;
    float construction_points;
    std::vector<Weapon> weapons;
    std::shared_ptr<ShipDesign> design;
    float ship_speed;

    float GetShipSpeed()
    {
        ship_speed = (energy >= engines_energy_consumtion ? engines_power
                                                          : engines_power * (energy / engines_energy_consumtion)) /
                     design->ship_size;
        return ship_speed;
    }

    void RegenShip()
    {
        energy += energy_regen;
        energy = std::max(energy, max_energy);
        hp += hp_regen;
        hp = std::max(hp, max_hp);
        float shield_regeneration = std::min(max_shield - shield, max_shield * shield_regen_percentage);
        float shield_regen_cost = shield_regeneration * shield_regen_energy_cost;
        if (energy >= shield_regen_cost)
        {
            shield += shield_regeneration;
            energy -= shield_regen_cost;
        }
        else
        {
            shield += energy * shield_regen_energy_cost_reverse;
            energy = 0.0f;
        }
    }

    void MoveShip(float distance) { energy -= engines_energy_consumtion * (distance / ship_speed); }
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
    float fleet_speed_per_tour = -1.0f;
    bool empty_fleet = false;  // if true then fleet needs to be deleted
    unsigned int owner_id;

    void UpdateFleet()
    {
        int to_delete = -1;
        do
        {
            to_delete = -1;
            for (int i = 0; i < ships.size(); i++)
            {
                if (ships[i]->hp <= 0)
                {
                    to_delete = i;
                    break;
                }
            }
            if (to_delete >= 0)
            {
                ships.erase(ships.begin() + to_delete);
            }
        } while (to_delete >= 0);

        if (ships.size() == 0)
        {
            empty_fleet = true;
            return;
        }

        for (const auto& ship : ships)
        {
            ship->RegenShip();
        }

        MoveFleet();
    }

    void UpdateFleetSpeed()
    {
        if (ships.size() == 0)
        {
            empty_fleet = true;
            return;
        }

        float minimum_speed = ships.front()->GetShipSpeed();
        for (const auto& ship : ships)
        {
            float speed = ship->GetShipSpeed();
            if (speed < minimum_speed)
                minimum_speed = speed;
        }

        fleet_speed_per_tour = minimum_speed;
    }

    void MoveFleet()
    {
        if (std::isnan(wanted_position.x) || wanted_position == position)
            return;
        UpdateFleetSpeed();
        if (fleet_speed_per_tour <= 0.0f)
            return;
        auto movement_vec = position - wanted_position;
        float movement_length = std::sqrtf(movement_vec.squaredLength());
        if (movement_length > fleet_speed_per_tour)
        {
            for (const auto& ship : ships)
            {
                ship->MoveShip(fleet_speed_per_tour);
            }
            movement_vec *= fleet_speed_per_tour / movement_length;
            position += movement_vec;
        }
        else
        {
            for (const auto& ship : ships)
            {
                ship->MoveShip(movement_length);
            }
            position += movement_vec;
        }
    }
};
