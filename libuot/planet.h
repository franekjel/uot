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
     {"Temperate climate", "This planet has Earth-like climate, good plants", {{Building::BuildingType::Farms, 6}}}},
    {PlanetaryFeatures::PlanetaryFeatureType::HotClimate,
     {"Hot climate",
      "This planet is very hot. Plants can only be grown near the poles",
      {{Building::BuildingType::Farms, 2}}}},
    {PlanetaryFeatures::PlanetaryFeatureType::ColdClimate,
     {"Cold climate",
      "This planet is very cold. Plants can only be grown near the equator",
      {{Building::BuildingType::Farms, 1}}}},
    {PlanetaryFeatures::PlanetaryFeatureType::MetalsDeposit,
     {"Metals deposit", "", {{Building::BuildingType::MetalsMine, 1}}}},
    {PlanetaryFeatures::PlanetaryFeatureType::RareMetalsDeposit,
     {"Rare metals deposit", "", {{Building::BuildingType::MetalsMine, 1}}}},
    {PlanetaryFeatures::PlanetaryFeatureType::CrystalsDeposit,
     {"Crystals deposit", "", {{Building::BuildingType::CrystalsMine, 1}}}},
    {PlanetaryFeatures::PlanetaryFeatureType::FertileLands,
     {"Fertile lands", "", {{Building::BuildingType::Farms, 1}}}},
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

struct Colony
{
    static float population_building_modificator;
    unsigned int id;
    std::shared_ptr<Planet> planet;
    std::map<Building::BuildingType, int> buildings;
    float population;
    float base_population_growth = 0.05f;
    float base_population_starving_death = 0.025f;
    std::shared_ptr<Player> owner;
    float unemployed_population;
    std::vector<BuildingBuildProgress> building_queue = {};

    static const Building& GetBuildingFromType(Building::BuildingType type) { return Buildings.at(type); }

    std::map<Resource, float> GetColonyGains();

    std::map<Resource, float> GetColonyExpenses();

    void UpdateBuildingQueue();

    void AddBuildingToQueue(Building::BuildingType type,
                            Building::BuildingType upgrade_from = Building::BuildingType::None);

    void RemoveBuildingFromQueueOnPosition(unsigned int position);

    std::map<Building::BuildingType, int> GetAvailableBuildings();

    Colony(const unsigned int id, const std::shared_ptr<Planet> planet_);

    // fields below are server-only
    bool population_changed;
    bool building_queue_changed;
    std::vector<BuildingBuildProgress> new_buildings;
};
