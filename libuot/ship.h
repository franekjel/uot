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
    int inside_size;
    std::map<Resource, float> cost;
    std::map<Resource, float> additional_upkeep;
    float hp;
    float speed;
    float engines_energy_consumtion;
    float crew;  // people needed to crew
    float worker_weeks_cost;
    float warp_drive_energy;  // energy need to warp
    int ship_aggro;
    const std::vector<Technology::TechnologyType> required_technologies;

    enum Type
    {
        SmallShipHull = 4,
        MediumShipHull = 5,
        GrandShipHull = 6
    };

    ShipHull(const int sides_size, const int inside_size, const std::map<Resource, float>& cost,
             const std::map<Resource, float>& additional_upkeep, const float hp, const float speed,
             const float engines_energy_consumtion, const float crew, const float worker_weeks_cost,
             const float warp_drive_energy, const int ship_aggro,
             const std::vector<Technology::TechnologyType> required_technologies);
};

const std::map<ShipHull::Type, ShipHull> ShipHulls{
    {ShipHull::SmallShipHull, ShipHull(4, 4, {{Resource::Metals, 50.0f}}, {{Resource::Antimatter, 0.5f}}, 80.0f, 0.04f,
                                       1.0f, 0.1f, 60.0f, 15.0f, 1, {Technology::TechnologyType::Spaceships})},
    {ShipHull::MediumShipHull,
     ShipHull(12, 12, {{Resource::Metals, 100.0f}, {Resource::RareMetals, 20.0f}}, {{Resource::Antimatter, 1.2f}},
              250.0f, 0.03f, 2.5f, 0.25f, 120.0f, 40.0f, 2, {Technology::TechnologyType::AdvancedSpaceships})},
    {ShipHull::GrandShipHull,
     ShipHull(36, 36, {{Resource::Metals, 200.0f}, {Resource::RareMetals, 40.0f}, {Resource::Polymers, 20.0f}},
              {{Resource::Antimatter, 2.5f}}, 700.0f, 0.02f, 4.0f, 0.5f, 250.0f, 80.0f, 4,
              {Technology::TechnologyType::SpaceCruisers})}

};

const std::map<ShipHull::Type, std::string> hull_type_to_design_name = {
    {ShipHull::SmallShipHull, "Small"}, {ShipHull::MediumShipHull, "Medium"}, {ShipHull::GrandShipHull, "Grand"}};

struct ShipDesign
{
    unsigned int id;
    std::string name;
    ShipHull::Type hull_type;
    std::map<ModuleType, int> sides;
    std::map<ModuleType, int> inside;
    std::map<Resource, float> cost;    // sum of costs of modules and hull
    std::map<Resource, float> upkeep;  // total upkeep
    float worker_weeks_cost;
    static constexpr float percentage_cost_upkeep = 0.05f;  // which part of ship cost goes to weekly upkeep

    ShipDesign(const unsigned int id, const std::string& name, const ShipHull::Type hull_type,
               const std::map<ModuleType, int>& sides, const std::map<ModuleType, int>& inside);
};

bool IsShipDesignCorrect(const ShipHull::Type hull_type, const std::map<ModuleType, int>& sides,
                         const std::map<ModuleType, int>& inside);
bool IsShipDesignPossible(const ShipHull::Type hull_type, const unsigned int sides_size,
                          const unsigned int inside_size);

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
    float warp_drive_charge = 0.0f;
    float full_warp_drive_charge = 100.0f;
    int ship_aggro = 0;
    std::shared_ptr<ShipDesign> design;
    std::shared_ptr<Fleet> fleet;

    std::map<ModuleType, int> ship_weapons;
    std::map<ModuleType, float> ship_weapons_current_state;
    float reload_modifier = -1.0f;

    static std::shared_ptr<Ship> ShipFromDesign(const int id, const std::shared_ptr<ShipDesign> design);

    float GetShipSpeed();

    void RegenShip();
    bool ChargeWarpDrive();

    void JumpShip();
    void MoveShip(float distance) { energy -= engines_energy_consumtion * (distance / speed); }
};

struct Fleet
{
    unsigned int id;
    std::map<unsigned int, std::shared_ptr<Ship>> ships;
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

    int fleet_aggro;
    std::map<ModuleType, std::pair<int, int>> fleet_weapons;

    std::shared_ptr<InhabitableObject> base_building_object;
    std::shared_ptr<Planet> colony_building_object;
    float building_progress;
    float full_building_progress;

    std::shared_ptr<Colony> invaded_colony;  // used only if there is a colony currently being invaded

    std::map<Weapon::SpecialFeatures, float> gained_damage = {
        {Weapon::SpecialFeatures::BypassShield, 0.0f},
        {Weapon::SpecialFeatures::HPDamageBonus, 0.0f},
        {Weapon::SpecialFeatures::None, 0.0f},
        {Weapon::SpecialFeatures::ShieldDamageBonus, 0.0f},
    };

    static constexpr float kNearValue = 0.01f;
    static constexpr float kColonizationCost = 10.0f;
    static constexpr float kBaseInvasionFightingNumber = 0.5f;
    static constexpr float kInvasionColonyWinProbability = 0.7f;
    static constexpr float kInvasionMaxWinnerDeathRate = 0.3f;
    static constexpr float kInvasionMaxLoserDeathRate = 1.0f;

    static constexpr float kEpsSoldiers = 0.001f;

    enum Action
    {
        None,
        WarpLoading,
        BuildAsteroidMine,
        Colonize,
        Invade,
        KickOutCivilians,
        KidnapCivilians,
        DisembarkSoldiers,
        BorrowSoldiers,
        CancelAction
    } current_action;

    static constexpr std::array<std::string_view, 10> kActionNames{"None",
                                                                   "Loading warp",
                                                                   "Building",
                                                                   "Colonizing",
                                                                   "Invading",
                                                                   "Disembark civilians",
                                                                   "Boarding civilians",
                                                                   "Disembark soldiers",
                                                                   "Boarding soldiers",
                                                                   "Cancel"};

    Fleet() = default;

    Fleet(const unsigned int id, const std::shared_ptr<Sector>& location_sector, const Point& position,
          const unsigned int owner_id);

    void UpdateFleet();

    void UpdateFleetSpeed();

    void AddShipToFleet(const std::shared_ptr<Ship>& ship);

    std::map<Resource, float> GetUpkeepCost();

    void CountDamage();

    void MoveFleet();
    void JumpFleet();

    void MoveCiviliansToColony(std::shared_ptr<Colony> colony);
    void MoveCiviliansFromColony(std::shared_ptr<Colony> colony);
    void MoveSoldiersToColony(std::shared_ptr<Colony> colony);
    void MoveSoldiersFromColony(std::shared_ptr<Colony> colony);

    void KillSoldiers(float number);

    void InvadeColony(std::shared_ptr<Colony> colony);

    // client only fields
    float current_hp;
    float max_hp;
    float current_shields;
    float max_shields;
    float average_energy;
    Point movement_vec;
};
