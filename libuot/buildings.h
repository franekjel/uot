#pragma once
#include <map>
#include <set>
#include <string>

#include "resource.h"

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
    Building::BuildingType::MilitaryTrainingCentre, Building::BuildingType::PowerPlants
};
