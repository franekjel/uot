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

        // Engineering and Spaceships are starting technologies

        Engineering,

        Spaceships,  // small hull, small shipyards, small reactor, metal armor plates, quarters, basic contruction
                     // moudule, small laser, railgun
        AdvancedSpaceships,  // medium shipyards, medium hull
        SpaceCruisers,       // grand shipyards, cruiser hull

        SoldiersTrainingProgram,  // military training centre

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

        NanobotsActivation,  // Nanobots excavation facility

        ImprovedReactor,  // improved small and big reactor
        AdvancedReactor,  // advanced small and big reactor

        EnergyShields,
        ImprovedEnergyShields,
        AdvancedEnergyShields,

        ImprovedArmor,
        AdvancedArmor,

        AdvancedSpaceEngineering,  // advanced contruction module
        NanobotsEngineering,       // nanobots contruction module and nanobots repair module

        ImprovedLaserTechnology,  // improved small laser, big lase
        AdvancedLaserTechnology,  // advanced big laser

        AntimatterProcessing,  // Antimatter cannon

        AdvancedRailgun,

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
                {Technology::TechnologyType::AdvancedMining, Technology::TechnologyType::SoldiersTrainingProgram,
                 Technology::TechnologyType::PolymersProcessing, Technology::TechnologyType::AdvancedGenetics,
                 Technology::TechnologyType::ResearchOrganisation, Technology::TechnologyType::ColdFusion})},

    {Technology::TechnologyType::Spaceships,
     Technology(
         1, "Spaceships", "Starting technology. Allows constructing base spaceships and base modules", 0,
         {Technology::TechnologyType::AdvancedSpaceships, Technology::TechnologyType::ImprovedReactor,
          Technology::TechnologyType::EnergyShields, Technology::TechnologyType::ImprovedArmor,
          Technology::TechnologyType::AdvancedSpaceEngineering, Technology::TechnologyType::ImprovedLaserTechnology,
          Technology::TechnologyType::AntimatterProcessing, Technology::TechnologyType::AdvancedRailgun})},
    {Technology::TechnologyType::AdvancedSpaceships,
     Technology(2, "Advanced Spaceships",
                "By utilizing better materials we can create biggers spaceships with more powerful modules. Allows "
                "contructing medium ships in medium orbital shipyards",
                300, {Technology::TechnologyType::SpaceCruisers})},
    {Technology::TechnologyType::SpaceCruisers,
     Technology(3, "Space cruiser", "Greatest space ships requires much more advanced technology", 3000, {})},

    {Technology::TechnologyType::SoldiersTrainingProgram,
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

    {Technology::TechnologyType::NanobotsActivation,
     Technology(38, "Nanobots activation",
                "During our exploration we encountered ancient nanobots. We can't build something this sophisticated "
                "but with the right technology we should be able to activate and use them. Allows construction of "
                "nanobots excavation facility on planets with ancient nanobots deposits",
                5000, {})},

    {Technology::TechnologyType::ImprovedReactor,
     Technology(39, "Improved reactors", "Improved reactors to provide more power to our spaceships", 500,
                {Technology::TechnologyType::AdvancedReactor})},
    {Technology::TechnologyType::AdvancedReactor,
     Technology(40, "Advanced reactors", "Advanced reactors are needed to power most powerful weapons", 2000, {})},

    {Technology::TechnologyType::EnergyShields,
     Technology(41, "Energy shields",
                "Our scientist claims that antimatter energy can be used to protect our spaceships", 500,
                {Technology::TechnologyType::ImprovedEnergyShields})},
    {Technology::TechnologyType::ImprovedEnergyShields,
     Technology(42, "Improved energy shields", "By improving energy transmission shield can endure more damage", 1000,
                {Technology::TechnologyType::AdvancedEnergyShields})},
    {Technology::TechnologyType::AdvancedEnergyShields,
     Technology(43, "Advanced energy shields", "Using dark matter can further improve our shields", 2500, {})},

    {Technology::TechnologyType::ImprovedArmor,
     Technology(44, "Improved armor", "Reinforcing our ships armor with rare metals will make them more durable", 500,
                {Technology::TechnologyType::AdvancedArmor})},
    {Technology::TechnologyType::AdvancedArmor,
     Technology(
         45, "Advanced armor",
         "Advanced armor utilizes complicated design of many layers and provide even more durability to our ships",
         1200, {})},

    {Technology::TechnologyType::AdvancedSpaceEngineering,
     Technology(46, "Advanced space enginnering",
                "Improved robots and tools will allow us to speed up space construction", 500,
                {Technology::TechnologyType::NanobotsEngineering})},
    {Technology::TechnologyType::NanobotsEngineering,
     Technology(
         47, "Nanobots enginnering",
         "Nanobots can be used in place of traditionals tools to contruct structures in space and repair our ships",
         3000, {})},

    {Technology::TechnologyType::ImprovedLaserTechnology,
     Technology(48, "Improved laser technology",
                "By improving our laser technologi we will be able to contruct more powerful battle lasers", 1000,
                {Technology::TechnologyType::AdvancedLaserTechnology})},
    {Technology::TechnologyType::AdvancedLaserTechnology,
     Technology(49, "Improved laser technology",
                "With most powerful lasers we will be able to destroy our enemies before they even approach our ships",
                3000, {})},

    {Technology::TechnologyType::AntimatterProcessing,
     Technology(50, "Antimatter provessing",
                "Antimatter is by nature very dangerous and we can utilize this in weapon design", 1000, {})},

    {Technology::TechnologyType::AdvancedRailgun,
     Technology(51, "Advanced railgun",
                "Improving energy transmission and bullet construction will our railgun to be more deadly", 1200, {})},

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
