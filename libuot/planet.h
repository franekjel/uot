#pragma once

#include <map>
#include <memory>
#include <set>

#include "resource.h"
#include "sectorobject.h"
#include "structs.h"

struct Building
{
    enum BuildingType
    {
        None,
        PlanetaryAdministration,
        ImprovedMetalsMine,
        MetalsMine,
        Farm,
        Greenhouses
    };

    const std::map<Resource, float> cost;
    const std::map<Resource, float> upkeep;
    const float workers;
    const std::map<Resource, float> production;

    const BuildingType upgrade;

    bool operator<(const Building& b) const  // whatever, needed to make sets and maps
    {
        return this->workers < b.workers;
    }
};

// all buildings
const std::map<Building::BuildingType, Building> Buildings{

    {Building::BuildingType::PlanetaryAdministration,
     {{{Resource::Metals, 50.0f}},
      {{Resource::Antimatter, 1.0f}, {Resource::Cryptocurrencies, 10.0f}},
      10.0f,
      {},
      Building::BuildingType::None}},

    {Building::BuildingType::ImprovedMetalsMine,
     {{{Resource::Metals, 50.0f}, {Resource::Crystals, 10.0f}},
      {{Resource::Antimatter, 2.0f}, {Resource::Cryptocurrencies, 15.0f}},
      15.0f,
      {{Resource::Metals, 20.0f}},
      Building::BuildingType::None}},

    {Building::BuildingType::MetalsMine,
     {{{Resource::Metals, 50.0f}},
      {{Resource::Antimatter, 1.0f}, {Resource::Cryptocurrencies, 10.0f}},
      10.0f,
      {{Resource::Metals, 10.0f}},
      Building::BuildingType::ImprovedMetalsMine}},

    {Building::BuildingType::Farm,
     {{{Resource::Metals, 30.0f}},
      {{Resource::Antimatter, 1.0f}, {Resource::Cryptocurrencies, 10.0f}},
      10.0f,
      {{Resource::Food, 10.0f}},
      Building::BuildingType::ImprovedMetalsMine}},

    {Building::BuildingType::Greenhouses,
     {{{Resource::Metals, 50.0f}},
      {{Resource::Antimatter, 3.0f}, {Resource::Cryptocurrencies, 12.0f}},
      10.0f,
      {{Resource::Food, 7.0f}},
      Building::BuildingType::None}},
};

// buildable buildings here
const std::set<Building::BuildingType> LimitedBuildings = {Building::BuildingType::Farm,
                                                           Building::BuildingType::MetalsMine};
const std::set<Building::BuildingType> UnlimitedBuildings = {Building::BuildingType::Greenhouses};

struct PlanetaryFeatures
{
    const std::map<Building::BuildingType, int> feature_buildings;
};

const PlanetaryFeatures TemperateClimate = {{{Building::BuildingType::Farm, 6}}};
const PlanetaryFeatures HotClimate = {{{Building::BuildingType::Farm, 2}}};
const PlanetaryFeatures ColdClimate = {{{Building::BuildingType::Farm, 2}}};
const PlanetaryFeatures SmallMetalsDeposits = {{{Building::BuildingType::MetalsMine, 2}}};
const PlanetaryFeatures MediumMetalsDeposits = {{{Building::BuildingType::MetalsMine, 4}}};
const PlanetaryFeatures BigMetalsDeposits = {{{Building::BuildingType::MetalsMine, 8}}};

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
    std::set<PlanetaryFeatures> planetary_features;
    std::map<Building::BuildingType, int> possible_buildings;
    std::shared_ptr<Colony> colony;

    Planet(const SectorObject& o, const PlanetClimate c, const std::set<PlanetaryFeatures>& f)
        : SectorObject(o), climate(c), planetary_features(f)
    {
        for (const auto& feature : planetary_features)
        {
            for (const auto& building : feature.feature_buildings)
            {
                possible_buildings[building.first] += building.second;
            }
        }
        size = int(20.0f * SectorObject::size);
    }
};

const float population_food_usage = 0.0f;  // TODO change to better value

struct Colony
{
    unsigned int id;
    std::shared_ptr<Planet> planet;
    std::map<Building::BuildingType, int> buildings;
    float population;
    bool population_changed;
    float base_population_growth = 0.05f;
    float base_population_starving_death = 0.025f;
    std::shared_ptr<Player> owner;

    static const Building& GetBuildingFromType(Building::BuildingType type) { return (*Buildings.find(type)).second; }

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

    Colony(const unsigned int id, const std::shared_ptr<Planet> planet_) : id(id)
    {
        population = 1;
        buildings = {};
        planet = planet_;
    }
};
