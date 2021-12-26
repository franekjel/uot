#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

struct Technology
{
    enum class TechnologyType
    {
        Empty,

        Engineering,

        Spaceships,         // small hull, small shipyards, basic modules
        AdvancedSpaceship,  // medium shipyards, medium hull
        SpaceCruisers,      // grand shipyards, cruiser hull

        SoldiersTraingingProgram,  // military training centre

        AdvancedMining,              // advanced mines
        AutomatedProcessingCenters,  // grand mines

        PolymersProcessing,  // polymers factory

        AdvancedGenetics,  // better farms & greenhouses

        ResearchOrganisation,  // research centre
        Polytechnic,

        ColdFusion,        // Fusion power plant
        DarkMatterEnergy,  // dark matter power plant

        SpaceMining1,  //+10% in asteroid mines per level
        SpaceMining2,
        SpaceMining3,
        SpaceMining4,
        SpaceMining5,

        PlanetaryMining1,  //+10% in planet mines and polymer factories per level
        PlanetaryMining2,
        PlanetaryMining3,
        PlanetaryMining4,
        PlanetaryMining5,

        FoodProcessing1,  //+10% to food production per level
        FoodProcessing2,
        FoodProcessing3,
        FoodProcessing4,
        FoodProcessing5,

        EnergyProduction1,  //+10% to energy production
        EnergyProduction2,
        EnergyProduction3,
        EnergyProduction4,
        EnergyProduction5,

        Medicine1,  //+15% to pop growth per level
        Medicine2,
        Medicine3,
        Medicine4,
        Medicine5,

        // TODO: technologies for modules

        // always leave as the last one
        None,  // special type for aborting research :D
    };

    unsigned int id;
    std::string name;
    std::string description;
    float cost;
    std::vector<TechnologyType> unlock;

    Technology(const int id, const std::string& name, const std::string& desc, const float cost,
               const std::vector<TechnologyType>& unlock)
        : id(id), name(name), description(desc), cost(cost), unlock(unlock)
    {
    }

    bool operator<(const Technology& t) const { return this->id < t.id; }
};

// all technologies
const std::map<Technology::TechnologyType, Technology> Technologies{
    {Technology::TechnologyType::Engineering,
     Technology(0, "Engineering", "Starting technology. Allows constructing base buildngs", 0,
                {Technology::TechnologyType::AdvancedMining, Technology::TechnologyType::SoldiersTraingingProgram,
                 Technology::TechnologyType::PolymersProcessing, Technology::TechnologyType::AdvancedGenetics,
                 Technology::TechnologyType::ResearchOrganisation, Technology::TechnologyType::ColdFusion})},

    {Technology::TechnologyType::Spaceships,
     Technology(1, "Spaceships", "Starting technology. Allows constructing base spaceships and base modules", 0,
                {Technology::TechnologyType::AdvancedSpaceship})},
    {Technology::TechnologyType::AdvancedSpaceship,
     Technology(2, "Advanced Spaceships",
                "By utilizing better materials we can create biggers spaceships with more powerful modules. Allows "
                "contructing medium ships in medium orbital shipyards",
                300, {Technology::TechnologyType::SpaceCruisers})},
    {Technology::TechnologyType::SpaceCruisers,
     Technology(3, "Space cruiser", "Greatest space ships requires much more advanced technology", 3000, {})},

    {Technology::TechnologyType::SoldiersTraingingProgram,
     Technology(4, "Soldiers trainging program",
                "By creating new soldiers trainging program we will be able to recruit elite troops", 300, {})},

    {Technology::TechnologyType::AdvancedMining,
     Technology(5, "Advanced mining", "New mining tools and contruction techniques allows creating advanced mines", 300,
                {Technology::TechnologyType::AutomatedProcessingCenters})},
    {Technology::TechnologyType::AutomatedProcessingCenters,
     Technology(6, "Automated processing centers",
                "New discoveries in robotics and AI allows us to create grand resource processing centers. Allows "
                "contructing grand mines",
                3000, {Technology::TechnologyType::SpaceMining1, Technology::TechnologyType::PlanetaryMining1})},

    {Technology::TechnologyType::PolymersProcessing,
     Technology(7, "Polymers processing",
                "More advanced industry and spaceships modules requires the invention of new polymers", 1000, {})},

    {Technology::TechnologyType::AdvancedGenetics,
     Technology(8, "Advanced genetics", "Advanced biology allow us to create much more efficient food sources", 3000,
                {})},

    {Technology::TechnologyType::ResearchOrganisation,
     Technology(9, "Research organisation",
                "Creating new organisation standards allow our scients to cooperate more efficiently", 2000,
                {Technology::TechnologyType::Polytechnic})},
    {Technology::TechnologyType::Polytechnic,
     Technology(10, "Polytechnic",
                "The most advanced organisation to perform most advanced research and traing *elite* of society", 4000,
                {})},

    {Technology::TechnologyType::ColdFusion,
     Technology(11, "Cold fusion", "Cold fusion can provide us much more energy than our current power plants", 1200,
                {Technology::TechnologyType::DarkMatterEnergy})},
    {Technology::TechnologyType::DarkMatterEnergy,
     Technology(12, "Dark matter energy",
                "Our scientists claim, that by utilizing weird properties of dark matters we can create even powerful "
                "energy sources",
                4000, {})},

    {Technology::TechnologyType::SpaceMining1,
     Technology(13, "Space mining 1",
                "By optimizing our tools and algorithm we can increase efficinecy of our space mines", 3000,
                {Technology::TechnologyType::SpaceMining2})},
    {Technology::TechnologyType::SpaceMining2,
     Technology(14, "Space mining 2",
                "By optimizing our tools and algorithm we can increase efficinecy of our space mines", 4000,
                {Technology::TechnologyType::SpaceMining3})},
    {Technology::TechnologyType::SpaceMining3,
     Technology(15, "Space mining 3",
                "By optimizing our tools and algorithm we can increase efficinecy of our space mines", 5000,
                {Technology::TechnologyType::SpaceMining4})},
    {Technology::TechnologyType::SpaceMining4,
     Technology(16, "Space mining 4",
                "By optimizing our tools and algorithm we can increase efficinecy of our space mines", 6000,
                {Technology::TechnologyType::SpaceMining5})},
    {Technology::TechnologyType::SpaceMining5,
     Technology(17, "Space mining 5",
                "By optimizing our tools and algorithm we can increase efficinecy of our space mines", 7000, {})},

    {Technology::TechnologyType::PlanetaryMining1, Technology(18, "Planetary mining 1",
                                                              "By optimizing our tools and algorithm we can increase "
                                                              "efficinecy of our planetary mines and polymer factories",
                                                              3000, {Technology::TechnologyType::PlanetaryMining2})},
    {Technology::TechnologyType::PlanetaryMining2, Technology(19, "Planetary mining 2",
                                                              "By optimizing our tools and algorithm we can increase "
                                                              "efficinecy of our planetary mines and polymer factories",
                                                              4000, {Technology::TechnologyType::PlanetaryMining3})},
    {Technology::TechnologyType::PlanetaryMining3, Technology(20, "Planetary mining 3",
                                                              "By optimizing our tools and algorithm we can increase "
                                                              "efficinecy of our planetary mines and polymer factories",
                                                              5000, {Technology::TechnologyType::PlanetaryMining4})},
    {Technology::TechnologyType::PlanetaryMining4, Technology(21, "Planetary mining 4",
                                                              "By optimizing our tools and algorithm we can increase "
                                                              "efficinecy of our planetary mines and polymer factories",
                                                              6000, {Technology::TechnologyType::PlanetaryMining5})},
    {Technology::TechnologyType::PlanetaryMining5, Technology(22, "Planetary mining 5",
                                                              "By optimizing our tools and algorithm we can increase "
                                                              "efficinecy of our planetary mines and polymer factories",
                                                              7000, {})},

    {Technology::TechnologyType::FoodProcessing1,
     Technology(23, "Food processing 1",
                "By optimizing our food processing and genetics we can increase "
                "efficinecy of our food sources",
                3000, {Technology::TechnologyType::FoodProcessing2})},
    {Technology::TechnologyType::FoodProcessing2,
     Technology(24, "Food processing 2",
                "By optimizing our food processing and genetics we can increase "
                "efficinecy of our food sources",
                4000, {Technology::TechnologyType::FoodProcessing3})},
    {Technology::TechnologyType::FoodProcessing3,
     Technology(25, "Food processing 3",
                "By optimizing our food processing and genetics we can increase "
                "efficinecy of our food sources",
                5000, {Technology::TechnologyType::FoodProcessing4})},
    {Technology::TechnologyType::FoodProcessing4,
     Technology(26, "Food processing 4",
                "By optimizing our food processing and genetics we can increase "
                "efficinecy of our food sources",
                6000, {Technology::TechnologyType::FoodProcessing5})},
    {Technology::TechnologyType::FoodProcessing5,
     Technology(27, "Food processing 5",
                "By optimizing our food processing and genetics we can increase "
                "efficinecy of our food sources",
                7000, {})},

    {Technology::TechnologyType::EnergyProduction1,
     Technology(28, "Energy production 1", "Optimization of our energy generation and processing", 3000,
                {Technology::TechnologyType::EnergyProduction2})},
    {Technology::TechnologyType::EnergyProduction2,
     Technology(29, "Energy production 2", "Optimization of our energy generation and processing", 4000,
                {Technology::TechnologyType::EnergyProduction3})},
    {Technology::TechnologyType::EnergyProduction3,
     Technology(30, "Energy production 3", "Optimization of our energy generation and processing", 5000,
                {Technology::TechnologyType::EnergyProduction4})},
    {Technology::TechnologyType::EnergyProduction4,
     Technology(31, "Energy production 4", "Optimization of our energy generation and processing", 6000,
                {Technology::TechnologyType::EnergyProduction5})},
    {Technology::TechnologyType::EnergyProduction5,
     Technology(32, "Energy production 5", "Optimization of our energy generation and processing", 7000, {})},

    {Technology::TechnologyType::Medicine1,
     Technology(33, "Medicine 1", "Better medicine means higher birth rate and longer live", 3000,
                {Technology::TechnologyType::Medicine2})},
    {Technology::TechnologyType::Medicine2,
     Technology(34, "Medicine 2", "Better medicine means higher birth rate and longer live", 4000,
                {Technology::TechnologyType::Medicine3})},
    {Technology::TechnologyType::Medicine3,
     Technology(35, "Medicine 3", "Better medicine means higher birth rate and longer live", 5000,
                {Technology::TechnologyType::Medicine4})},
    {Technology::TechnologyType::Medicine4,
     Technology(36, "Medicine 4", "Better medicine means higher birth rate and longer live", 6000,
                {Technology::TechnologyType::Medicine5})},
    {Technology::TechnologyType::Medicine5,
     Technology(37, "Medicine 5", "Better medicine means higher birth rate and longer live", 7000, {})},

};

struct TechnologyProgress
{
    Technology::TechnologyType technology;
    float progress_left;

    TechnologyProgress() : technology(Technology::TechnologyType::None), progress_left(-10.0f) {}

    TechnologyProgress(Technology::TechnologyType technology_)
        : technology(technology_), progress_left(Technologies.at(technology_).cost)
    {
    }

    operator bool() const
    {
        return technology != Technology::TechnologyType::Empty && technology != Technology::TechnologyType::None;
    }
};
