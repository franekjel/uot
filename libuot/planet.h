#pragma once

#include <map>
#include <memory>
#include <set>

#include "buildings.h"
#include "resource.h"
#include "sectorobject.h"
#include "structs.h"

struct PlanetaryFeatures
{
    enum PlanetaryFeatureType
    {
        TemperateClimate,
        HotClimate,
        ColdClimate,
        MetalsDeposit,
        RareMetalsDeposit,
        CrystalsDeposit,
        FertileLands,
        AncientRuins,
        AncientNanobotsDeposit,
    };
    const std::string name;
    const std::string description;
    const std::map<Building::BuildingType, int> feature_buildings;
};

// all features
const std::map<PlanetaryFeatures::PlanetaryFeatureType, PlanetaryFeatures> PlanetaryFeaturesTypes{
    {PlanetaryFeatures::PlanetaryFeatureType::TemperateClimate,
     {"Temperate climate",
      "This planet has Earth-like climate, good plants",
      {{Building::BuildingType::Farms, 6}, {Building::BuildingType::AdvancedFarms, 6}}}},
    {PlanetaryFeatures::PlanetaryFeatureType::HotClimate,
     {"Hot climate",
      "This planet is very hot. Plants can only be grown near the poles",
      {{Building::BuildingType::Farms, 2}, {Building::BuildingType::AdvancedFarms, 2}}}},
    {PlanetaryFeatures::PlanetaryFeatureType::ColdClimate,
     {"Cold climate",
      "This planet is very cold. Plants can only be grown near the equator",
      {{Building::BuildingType::Farms, 1}, {Building::BuildingType::AdvancedFarms, 1}}}},
    {PlanetaryFeatures::PlanetaryFeatureType::MetalsDeposit,
     {"Metals deposit",
      "",
      {{Building::BuildingType::MetalsMine, 1},
       {Building::BuildingType::AdvancedMetalsMine, 1},
       {Building::BuildingType::GrandMetalsMine, 1}}}},
    {PlanetaryFeatures::PlanetaryFeatureType::RareMetalsDeposit,
     {"Rare metals deposit",
      "",
      {{Building::BuildingType::RareMetalsMine, 1},
       {Building::BuildingType::AdvancedRareMetalsMine, 1},
       {Building::BuildingType::GrandRareMetalsMine, 1}}}},
    {PlanetaryFeatures::PlanetaryFeatureType::CrystalsDeposit,
     {"Crystals deposit",
      "",
      {{Building::BuildingType::CrystalsMine, 1},
       {Building::BuildingType::AdvancedCrystalsMine, 1},
       {Building::BuildingType::GrandCrystalsMine, 1}}}},
    {PlanetaryFeatures::PlanetaryFeatureType::FertileLands,
     {"Fertile lands", "", {{Building::BuildingType::Farms, 1}, {Building::BuildingType::AdvancedFarms, 1}}}},
    {PlanetaryFeatures::PlanetaryFeatureType::AncientRuins,
     {"Ancient ruins",
      "Remains of ancient civilization can be found on this planet",
      {{Building::BuildingType::ArchaeologicalSite, 1}}}},
    {PlanetaryFeatures::PlanetaryFeatureType::AncientNanobotsDeposit,
     {"Ancient nanobots deposits",
      "On this planet can be found ancient nanbots. They are inactive, but with appropriate technology it will be "
      "possible to use them",
      {{Building::BuildingType::NanobotsExcavationFacility, 1}}}},
};

// habitable planet
struct Planet : SectorObject
{
    enum class PlanetClimate
    {
        Temperate,  // Earth-like
        Cold,       // a bit colder
        Hot,        // a bit hotter
    };
    PlanetClimate climate;
    int size;
    std::map<PlanetaryFeatures::PlanetaryFeatureType, int> planetary_features;
    std::map<Building::BuildingType, int> possible_buildings;
    std::shared_ptr<Colony> colony;
    bool is_being_colonized = false;
    static constexpr float colony_work_cost = 500.0f;
    static constexpr float colony_metal_cost = 500.0f;

    Planet(const SectorObject& o, const PlanetClimate c,
           const std::map<PlanetaryFeatures::PlanetaryFeatureType, int>& f)
        : SectorObject(o), climate(c), planetary_features(f)
    {
        for (const auto& features : planetary_features)
        {
            for (const auto& building : PlanetaryFeaturesTypes.at(features.first).feature_buildings)
            {
                possible_buildings[building.first] += building.second * features.second;
            }
        }
        size = int(20.0f * SectorObject::size);
    }
};

const float population_food_usage = 0.1f;
const float base_population = 1.0f;

struct BuildingBuildProgress
{
    Building::BuildingType type;
    float worker_week_units_left;
    Building::BuildingType upgrade_of;

    BuildingBuildProgress()
        : type(Building::BuildingType::None), upgrade_of(Building::BuildingType::None), worker_week_units_left(0.0f){};

    BuildingBuildProgress(Building::BuildingType new_building,
                          Building::BuildingType upgraded_building = Building::BuildingType::None)
        : type(new_building), upgrade_of(upgraded_building)
    {
        worker_week_units_left = Buildings.at(new_building).worker_weeks_cost;
    }
};

struct ShipBuildProgress
{
    std::shared_ptr<ShipDesign> design;
    std::shared_ptr<Sector> sector;
    std::shared_ptr<Ship> ship;
    float worker_week_units_left;
    bool new_fleet;
    ShipBuildProgress()
        : design(nullptr), worker_week_units_left(0.0f), sector(nullptr), ship(nullptr), new_fleet(false){};

    ShipBuildProgress(const std::shared_ptr<ShipDesign>& design, const std::shared_ptr<Sector>& sector);
};

struct Colony
{
    static float population_building_modificator;
    unsigned int id;
    std::shared_ptr<Planet> planet;
    std::map<Building::BuildingType, int> buildings;
    float population;
    float soldiers = 0.0f;
    float base_population_growth = 0.01f;
    float base_population_starving_death = 0.005f;
    std::shared_ptr<Player> owner;
    float unemployed_population;
    float buildings_working_modifier;
    std::vector<BuildingBuildProgress> building_queue = {};
    std::vector<ShipBuildProgress> ship_building_queue = {};

    static const Building& GetBuildingFromType(Building::BuildingType type) { return Buildings.at(type); }

    std::map<Resource, float> GetColonyGains();

    std::map<Resource, float> GetColonyExpenses();

    void UpdateBuildingQueue();
    void UpdateShipBuildingQueue();

    void AddBuildingToQueue(Building::BuildingType type,
                            Building::BuildingType upgrade_from = Building::BuildingType::None);

    std::map<Building::BuildingType, int> GetAvailableBuildings();

    Colony() = default;
    Colony(const unsigned int id, const std::shared_ptr<Planet> planet_);

    // fields below are server-only
    bool population_changed;
    bool soldiers_changed;
    bool building_queue_changed;
    bool ship_building_queue_changed;
    std::vector<BuildingBuildProgress> new_buildings;
    std::vector<ShipBuildProgress> new_ships;
};
