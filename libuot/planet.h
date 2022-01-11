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

    std::map<Building::BuildingType, int> GetAvailableBuildings(std::shared_ptr<Player> player);
};

const float population_food_usage = 0.1f;

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

    std::map<Resource, float> GetColonyGains()
    {
        std::map<Resource, float> colony_gains = {};
        int neccessary_workers = 0;

        for (auto& buildingType : buildings)
        {
            auto& building = Colony::GetBuildingFromType(buildingType.first);
            int number_of_buildings = buildingType.second;
            neccessary_workers += number_of_buildings * building.workers;
            for (auto& gains : building.production)
            {
                if (colony_gains.count(gains.first) == 0)
                    colony_gains[gains.first] = gains.second * number_of_buildings;
                else
                    colony_gains[gains.first] += gains.second * number_of_buildings;
            }
        }

        float colony_efficency = neccessary_workers > population ? population / neccessary_workers : 1.0f;
        unemployed_population = population - neccessary_workers;

        if (colony_efficency < 1.0f)
            colony_gains = colony_gains * colony_efficency;

        return colony_gains;
    }

    std::map<Resource, float> GetColonyExpenses()
    {
        std::map<Resource, float> colony_expenses = {};
        int neccessary_workers = 0;

        for (auto& buildingType : buildings)
        {
            auto& building = Colony::GetBuildingFromType(buildingType.first);
            int number_of_buildings = buildingType.second;
            neccessary_workers += number_of_buildings * building.workers;

            for (auto& expense : building.upkeep)
            {
                if (colony_expenses.count(expense.first) == 0)
                    colony_expenses[expense.first] = expense.second * number_of_buildings;
                else
                    colony_expenses[expense.first] += expense.second * number_of_buildings;
            }
        }

        float colony_efficency = neccessary_workers > population ? population / neccessary_workers : 1.0f;

        if (colony_efficency < 1.0f)
            colony_expenses = colony_expenses * colony_efficency;
        // I assumed that if building is less efficient it is also cheaper to upkeep

        return colony_expenses;
    }

    void UpdateBuildingQueue()
    {
        if (building_queue.size() == 0)
            return;

        float people_weeks_to_distribute = unemployed_population * population_building_modificator;
        auto itr = building_queue.begin();
        while (people_weeks_to_distribute > 0.0f && itr != building_queue.end())
        {
            if (itr->worker_week_units_left < people_weeks_to_distribute)
            {
                people_weeks_to_distribute -= itr->worker_week_units_left;
                itr->worker_week_units_left = 0.0f;
            }
            else
            {
                itr->worker_week_units_left -= people_weeks_to_distribute;
                people_weeks_to_distribute = 0.0f;
            }
            itr++;
        }

        while (building_queue.size() != 0 && building_queue.front().worker_week_units_left == 0.0f)
        {
            if (building_queue.front().upgrade_of != Building::BuildingType::None)
                buildings[building_queue.front().upgrade_of]--;
            buildings[building_queue.front().type]++;
            building_queue.erase(building_queue.begin());
        }

        building_queue_changed = true;
        return;
    }

    void AddBuildingToQueue(Building::BuildingType type,
                            Building::BuildingType upgrade_from = Building::BuildingType::None)
    {
        building_queue.push_back({type, upgrade_from});
        building_queue_changed = true;
    }

    void RemoveBuildingFromQueueOnPosition(unsigned int position)
    {
        if (position < building_queue.size())
            building_queue.erase(building_queue.begin() + position);
        building_queue_changed = true;
    }

    Colony(const unsigned int id, const std::shared_ptr<Planet> planet_) : id(id)
    {
        population = 1;
        buildings = {};
        planet = planet_;
        unemployed_population = population;
        building_queue = {};
        building_queue_changed = false;
    }

    // fields below are server-only
    bool population_changed;
    bool building_queue_changed;
};
