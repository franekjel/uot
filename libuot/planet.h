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
        MilitaryTrainingCentre,
        // ship contruction
        SmallOrbitalShipyard,
        MediumOrbitalShipyard,
        GrandOrbitalShipyard,

        // metals
        MetalsMine,
        AdvancedMetalsMine,
        GrandMetalsMine,
        // rare metals
        RareMetalsMine,
        AdvancedRareMetalsMine,
        GrandRareMetalsMine,
        // crystlas
        CrystalsMine,
        AdvancedCrystalsMine,
        GrandCrystalsMine,
        // polymers
        PolymersFactory,
        // food
        Farms,
        AdvancedFarms,
        // food but worser
        Greenhouses,
        FoodPruductionCentre,
        // energy (antimatter)
        PowerPlants,
        FusionPowerPlants,
        DarkMatterPowerPlants,
        // technology
        Laboratory,
        ResearchCentre,
        Polytechnic,
        // ancient relics
        ArchaeologicalSite,
        // ancient nanobots
        NanobotsExcavationFacility,
    };

    const std::string name;
    const std::string description;
    const std::map<Resource, float> cost;
    const std::map<Resource, float> upkeep;
    const float workers;
    const std::map<Resource, float> production;
    const float worker_weeks_cost;

    const BuildingType upgrade;

    bool operator<(const Building& b) const  // whatever, needed to make sets and maps
    {
        return this->workers < b.workers;
    }
};

// all buildings
const std::map<Building::BuildingType, Building> Buildings{

    {Building::BuildingType::PlanetaryAdministration,
     {"Planetary Administration",
      "Planetary administration offices from where the entire colony is controlled",
      {{Resource::Metals, 200.0f}},
      {{Resource::Antimatter, 5.0f}},
      10.0f,
      {},
      300.0f,
      Building::BuildingType::None}},

    {Building::BuildingType::MilitaryTrainingCentre,
     {"Military training centre",
      "Allows recruting soldiers",
      {{Resource::Metals, 200.0f}, {Resource::RareMetals, 40.0f}},
      {{Resource::Antimatter, 5.0f}},
      10.0f,
      {},
      300.0f,
      Building::BuildingType::None}},

    // ships
    {Building::BuildingType::SmallOrbitalShipyard,
     {"Small orbital shipyard",
      "Basic shipyard, can construct small spaceships",
      {{Resource::Metals, 300.0f}},
      {{Resource::Antimatter, 5.0f}},
      10.0f,
      {},
      300.0f,
      Building::BuildingType::None}},

    {Building::BuildingType::MediumOrbitalShipyard,
     {"Medium orbital shipyard",
      "More advanced shipyard allows contruction medium spaceships",
      {{Resource::Metals, 300.0f}, {Resource::RareMetals, 100.0f}, {Resource::Crystals, 50.0f}},
      {{Resource::Antimatter, 10.0f}},
      20.0f,
      {},
      400.0f,
      Building::BuildingType::GrandOrbitalShipyard}},

    {Building::BuildingType::GrandOrbitalShipyard,
     {"Grand orbital shipyard",
      "Grand orbital cuntruction needed to create greatest space ships",
      {{Resource::Metals, 500.0f},
       {Resource::RareMetals, 200.0f},
       {Resource::Crystals, 100.0f},
       {Resource::Polymers, 100.0f}},
      {{Resource::Antimatter, 10.0f}, {Resource::RareMetals, 2.0f}, {Resource::Crystals, 2.0f}},
      30.0f,
      {},
      600.0f,
      Building::BuildingType::None}},

    // metals
    {Building::BuildingType::MetalsMine,
     {"Metals mine",
      "Provides metals. May be further improved",
      {{Resource::Metals, 50.0f}},
      {{Resource::Antimatter, 1.0f}},
      10.0f,
      {{Resource::Metals, 5.0f}},
      300.0f,
      Building::BuildingType::AdvancedMetalsMine}},

    {Building::BuildingType::AdvancedMetalsMine,
     {"Advanced metals mine",
      "That metals mine use more technically advanced machines to produce more metals",
      {{Resource::Metals, 100.0f}, {Resource::RareMetals, 30.0f}},
      {{Resource::Antimatter, 2.5f}, {Resource::RareMetals, 0.5f}},
      15.0f,
      {{Resource::Metals, 12.0f}},
      400.0f,
      Building::BuildingType::GrandMetalsMine}},

    {Building::BuildingType::GrandMetalsMine,
     {"Grand metals mine",
      "By using technology normally used in space mining that mine can deliver gigantic amount of metals",
      {{Resource::Metals, 200.0f}, {Resource::RareMetals, 50.0f}, {Resource::Crystals, 20.0f}},
      {{Resource::Antimatter, 5.0f}, {Resource::Crystals, 0.5f}},
      20.0f,
      {{Resource::Metals, 20.0f}, {Resource::RareMetals, 0.5f}},
      500.0f,
      Building::BuildingType::None}},

    // rare metals
    {Building::BuildingType::RareMetalsMine,
     {"Rare metals mine",
      "This mine extracts rare metals",
      {{Resource::Metals, 200.0f}},
      {{Resource::Antimatter, 1.5f}},
      10.0f,
      {{Resource::RareMetals, 3.0f}},
      300.0f,
      Building::BuildingType::AdvancedRareMetalsMine}},

    {Building::BuildingType::AdvancedRareMetalsMine,
     {"Advanced rare metals mine",
      "Better metals refining method allow us to increase rare metals production",
      {{Resource::Metals, 300.0f}, {Resource::RareMetals, 50.0f}, {Resource::Crystals, 20.0f}},
      {{Resource::Antimatter, 3.0f}, {Resource::Crystals, 0.5f}},
      15.0f,
      {{Resource::RareMetals, 8.0f}},
      400.0f,
      Building::BuildingType::GrandRareMetalsMine}},

    {Building::BuildingType::GrandRareMetalsMine,
     {"Grand rare metals mine",
      "Advanced technology ensure that every particle of metal will be excavated",
      {{Resource::Metals, 400.0f},
       {Resource::RareMetals, 50.0f},
       {Resource::Crystals, 50.0f},
       {Resource::Polymers, 50.0f}},
      {{Resource::Antimatter, 5.0f}, {Resource::Crystals, 0.5f}, {Resource::Polymers, 0.5f}},
      20.0f,
      {{Resource::RareMetals, 12.0f}},
      500.0f,
      Building::BuildingType::None}},

    // crystals
    {Building::BuildingType::CrystalsMine,
     {"Crystal mine",
      "This mine extracts rare crystal needed by advanced building and modules",
      {{Resource::Metals, 200.0f}, {Resource::RareMetals, 50.0f}},
      {{Resource::Antimatter, 1.5f}},
      10.0f,
      {{Resource::Crystals, 3.0f}},
      300.0f,
      Building::BuildingType::AdvancedCrystalsMine}},

    {Building::BuildingType::AdvancedCrystalsMine,
     {"Advanced crystal mine",
      "By providing better tools more crystals can be safely extracted",
      {{Resource::Metals, 300.0f}, {Resource::RareMetals, 50.0f}, {Resource::Polymers, 40.0f}},
      {{Resource::Antimatter, 3.0f}, {Resource::RareMetals, 0.5f}},
      15.0f,
      {{Resource::Crystals, 8.0f}},
      400.0f,
      Building::BuildingType::GrandCrystalsMine}},

    {Building::BuildingType::GrandCrystalsMine,
     {"Grand crystals mine",
      "Extensive mines integrated with crystals processing centers can provide significant amounts of crystals",
      {{Resource::Metals, 400.0f}, {Resource::RareMetals, 20.0f}, {Resource::Polymers, 20.0f}},
      {{Resource::Antimatter, 5.0f}, {Resource::RareMetals, 1.0f}, {Resource::Polymers, 0.5f}},
      20.0f,
      {{Resource::Crystals, 12.0f}},
      500.0f,
      Building::BuildingType::None}},

    // polymer
    {Building::BuildingType::PolymersFactory,
     {"Polymers factory",
      "That factory specializes in producing durable polymers needed by our industry and military",
      {{Resource::Metals, 200.0f}, {Resource::RareMetals, 20.0f}, {Resource::Crystals, 20.0f}},
      {{Resource::Antimatter, 3.0f}, {Resource::RareMetals, 0.5f}, {Resource::Crystals, 0.5f}},
      25.0f,
      {{Resource::Polymers, 5.0f}},
      400.0f,
      Building::BuildingType::None}},

    // food
    {Building::BuildingType::Farms,
     {"Farms",
      "The good climate on this planet allows the construction of traditional farms, which are cheap to maintain",
      {{Resource::Metals, 50.0f}},
      {{Resource::Antimatter, 0.5f}},
      10.0f,
      {{Resource::Food, 6.0f}},
      300.0f,
      Building::BuildingType::AdvancedFarms}},

    {Building::BuildingType::AdvancedFarms,
     {"Advanced farms",
      "By implementing new biological discoveries food production of these farms can be further increased",
      {{Resource::Metals, 100.0f}, {Resource::RareMetals, 20.0f}, {Resource::Polymers, 20.0f}},
      {{Resource::Antimatter, 2.0f}},
      15.0f,
      {{Resource::Metals, 15.0f}},
      400.0f,
      Building::BuildingType::None}},

    {Building::BuildingType::Greenhouses,
     {"Greenhouses",
      "In these building plants are protected from the harsh climate",
      {{Resource::Metals, 300.0f}},
      {{Resource::Antimatter, 1.5f}},
      10.0f,
      {{Resource::Food, 6.0f}},
      300.0f,
      Building::BuildingType::FoodPruductionCentre}},

    {Building::BuildingType::FoodPruductionCentre,
     {"Greenhouses",
      "Grand underground facility for farming and food processing using latest technology",
      {{Resource::Metals, 400.0f}, {Resource::RareMetals, 400.0f}, {Resource::Crystals, 400.0f}},
      {{Resource::Antimatter, 2.5f}, {Resource::Crystals, 1.0f}},
      15.0f,
      {{Resource::Food, 15.0f}},
      400.0f,
      Building::BuildingType::None}},

    {Building::BuildingType::PowerPlants,
     {"Power plants",
      "Basic power plants utilizing varius techniques to gather energy and transform it to antimatter",
      {{Resource::Metals, 100.0f}},
      {},
      10.0f,
      {{Resource::Antimatter, 15.0f}},
      200.0f,
      Building::BuildingType::FusionPowerPlants}},

    {Building::BuildingType::FusionPowerPlants,
     {"Fusion power plants",
      "Technological advancements allows to use fusion energy like our in home",
      {{Resource::Metals, 200.0f}, {Resource::RareMetals, 50.0f}, {Resource::Crystals, 30.0f}},
      {{Resource::Crystals, 1.0f}},
      15.0f,
      {{Resource::Antimatter, 30.0f}},
      400.0f,
      Building::BuildingType::FusionPowerPlants}},

    {Building::BuildingType::DarkMatterPowerPlants,
     {"Dark matter power plants",
      "Only few scientist know how it works, but it works and produces gigantic amount of energy",
      {{Resource::Metals, 300.0f}, {Resource::RareMetals, 100.0f}, {Resource::Crystals, 100.0f}},
      {{Resource::DarkMatter, 1.0f}},
      15.0f,
      {{Resource::Antimatter, 60.0f}},
      500.0f,
      Building::BuildingType::None}},

    {Building::BuildingType::Laboratory,
     {"Labolatory",
      "A few scientists and some equipment to start developing our technology",
      {{Resource::Metals, 200.0f}},
      {{Resource::Antimatter, 2.0f}},
      10.0f,
      {{Resource::Technology, 10.0f}},
      300.0f,
      Building::BuildingType::ResearchCentre}},

    {Building::BuildingType::ResearchCentre,
     {"Research centre",
      "Big research facility with our best scientist and expensive equipement",
      {{Resource::Metals, 200.0f},
       {Resource::RareMetals, 30.0f},
       {Resource::Crystals, 30.0f},
       {Resource::Polymers, 30.0f}},
      {{Resource::Antimatter, 6.0f}, {Resource::Crystals, 0.5f}, {Resource::RareMetals, 0.5f}},
      15.0f,
      {{Resource::Technology, 25.0f}},
      400.0f,
      Building::BuildingType::None}},

    {Building::BuildingType::Polytechnic,
     {"Polytechnic",
      "*Elite* scientis focused on *elite* fields can provide us most *elite* technology",
      {{Resource::Metals, 200.0f},
       {Resource::RareMetals, 50.0f},
       {Resource::Crystals, 50.0f},
       {Resource::Polymers, 50.0f}},
      {{Resource::Antimatter, 8.0f},
       {Resource::Crystals, 1.0f},
       {Resource::RareMetals, 1.0f},
       {Resource::DarkMatter, 1.0f}},
      15.0f,
      {{Resource::Technology, 60.0f}},
      500.0f,
      Building::BuildingType::None}},

    {Building::BuildingType::ArchaeologicalSite,
     {"Archeological site",
      "Excavations performed to discover ancient relics and technologies",
      {{Resource::Metals, 100.0f}, {Resource::Crystals, 10.0f}, {Resource::Polymers, 10.0f}},
      {{Resource::Antimatter, 2.0f}, {Resource::Crystals, 0.5f}},
      10.0f,
      {{Resource::Technology, 20.0f}, {Resource::AncientRelics, 1.0f}},
      200.0f,
      Building::BuildingType::None}},

    {Building::BuildingType::NanobotsExcavationFacility,
     {"Nanobots excavation facility",
      "In this facility undamaged nanobots are identified nad activated",
      {{Resource::Metals, 200.0f}, {Resource::Crystals, 50.0f}, {Resource::RareMetals, 30.0f}},
      {{Resource::Antimatter, 4.0f}, {Resource::Crystals, 1.0f}},
      15.0f,
      {{Resource::AncientNanobots, 1.0f}},
      300.0f,
      Building::BuildingType::None}},

};

// Building which can be always build
const std::set<Building::BuildingType> UnlimitedBuildings = {
    Building::BuildingType::Greenhouses, Building::BuildingType::Laboratory,
    Building::BuildingType::MilitaryTrainingCentre, Building::BuildingType::PowerPlants};

struct PlanetaryFeatures
{
    enum PlanetaryFeatureType
    {
        TemperateClimate,
        HotClimate,
        ColdClimate,
        SmallMetalsDeposits,
        MediumMetalsDeposits,
        BigMetalsDeposits
    };
    const std::map<Building::BuildingType, int> feature_buildings;
};

// all features
const std::map<PlanetaryFeatures::PlanetaryFeatureType, PlanetaryFeatures> PlanetaryFeaturesTypes{
    {PlanetaryFeatures::PlanetaryFeatureType::TemperateClimate, {{{Building::BuildingType::Farms, 6}}}},
    {PlanetaryFeatures::PlanetaryFeatureType::HotClimate, {{{Building::BuildingType::Farms, 2}}}},
    {PlanetaryFeatures::PlanetaryFeatureType::ColdClimate, {{{Building::BuildingType::Farms, 2}}}},
    {PlanetaryFeatures::PlanetaryFeatureType::SmallMetalsDeposits, {{{Building::BuildingType::MetalsMine, 2}}}},
    {PlanetaryFeatures::PlanetaryFeatureType::MediumMetalsDeposits, {{{Building::BuildingType::MetalsMine, 4}}}},
    {PlanetaryFeatures::PlanetaryFeatureType::BigMetalsDeposits, {{{Building::BuildingType::MetalsMine, 8}}}}};

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
    std::set<PlanetaryFeatures::PlanetaryFeatureType> planetary_features;
    std::map<Building::BuildingType, int> possible_buildings;
    std::shared_ptr<Colony> colony;

    Planet(const SectorObject& o, const PlanetClimate c, const std::set<PlanetaryFeatures::PlanetaryFeatureType>& f)
        : SectorObject(o), climate(c), planetary_features(f)
    {
        for (const auto& feature : planetary_features)
        {
            for (const auto& building : PlanetaryFeaturesTypes.at(feature).feature_buildings)
            {
                possible_buildings[building.first] += building.second;
            }
        }
        size = int(20.0f * SectorObject::size);
    }
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
