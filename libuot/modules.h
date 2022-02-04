#pragma once

#include <map>
#include <memory>
#include <optional>
#include <string>
#include <utility>

#include <optional>
#include "resource.h"

struct Weapon
{
    int attack_count;
    float damage;
    float range;

    enum SpecialFeatures
    {
        None = 0,
        BypassShield = 1,
        ShieldDamageBonus = 2,
        HPDamageBonus = 4,
    } special_features;

    Weapon() = default;

    Weapon(const int attack_count, const float damage, const float range, const SpecialFeatures special_features)
        : attack_count(attack_count), damage(damage), range(range), special_features(special_features)
    {
    }
};

struct Module
{
    std::string name;
    int size;
    float energy_usage;
    std::map<Resource, float> cost;
    std::map<Resource, float> additional_upkeep;  // base upkeep of module is percentage of it's costs (amortization!)
    static constexpr float worker_weeks_cost_per_size = 5.0f;
    enum ModuleDestination
    {
        None = 0,
        Sides,
        Back,
        Inside,
    } destination;

    float generating_energy;       // reactors only
    float energy_capacity;         // reactors only
    float shield_capacity;         // shields only
    float additional_hp;           // armor only
    float contruction_speed;       // construction modules only
    float human_capacity;          // quarters only
    std::optional<Weapon> weapon;  // weapons only
    static constexpr float nanobots_hp_regen_amount = 20.0f;

    Module(const std::string& name, const int size, const float energy_usage, const std::map<Resource, float>& cost,
           const std::map<Resource, float>& additional_upkeep, const ModuleDestination destination)
        : name(name),
          size(size),
          energy_usage(energy_usage),
          cost(cost),
          additional_upkeep(additional_upkeep),
          destination(destination)
    {
    }

    static Module Reactor(Module m, const float generating_energy)
    {
        m.generating_energy = generating_energy;
        m.energy_capacity = generating_energy * 20.0f;
        return m;
    }

    static Module ShieldGenerator(Module m, const float shield_capacity)
    {
        m.shield_capacity = shield_capacity;
        return m;
    }
    static Module ArmorPlates(Module m, const float additional_hp)
    {
        m.additional_hp = additional_hp;
        return m;
    }
    static Module ContructionModule(Module m, const float contruction_speed)
    {
        m.contruction_speed = contruction_speed;
        return m;
    }
    static Module Quarters(Module m, const float human_capacity)
    {
        m.human_capacity = human_capacity;
        return m;
    }
    static Module WeaponModule(Module m, const Weapon weapon)
    {
        m.weapon = weapon;
        return m;
    }
};

enum class ModuleType
{
    SmallReactor,
    ImprovedSmallReactor,
    AdvancedSmallReactor,

    ImprovedBigReactor,
    AdvancedBigReactor,

    BasicShieldGenerator,
    ImprovedShieldGenerator,
    AdvancedShieldGenerator,

    MetalArmorPlates,
    ImprovedArmorPlates,
    AdvancedArmorPlates,

    Quarters,

    BasicContructionModule,
    AdvancedContructionModule,
    NanobotsContructionModule,

    NanobotsSelfRepairModule,

    SmallLaser,  // medium distance, bonus to shield damage
    ImprovedSmallLaser,

    BigLaser,  // big distance
    AdvancedBigLaser,

    AntimatterCannon,  // medium distance, bonus to hp damage

    Railgun,  // medium distance
    AdvancedRailgun,
};

// Module(const std::string& name, const int size, const float energy_usage, const std::map<Resource, float>& cost,
//       const std::map<Resource, float>& additional_upkeep, const ModuleDestination destination)

const std::map<ModuleType, Module> Modules{

    {ModuleType::SmallReactor, Module::Reactor(Module("Basic small reactor", 2, 0.0f, {{Resource::Metals, 5.0f}},
                                                      {{Resource::Antimatter, 1.0f}}, Module::Inside),
                                               5.0f)},

    {ModuleType::ImprovedSmallReactor,
     Module::Reactor(Module("Improved small reactor", 2, 0.0f,
                            {{Resource::Metals, 5.0f}, {Resource::RareMetals, 3.0f}, {Resource::Crystals, 3.0f}},
                            {{Resource::Antimatter, 2.0f}}, Module::Inside),
                     12.0f)},
    {ModuleType::ImprovedBigReactor,
     Module::Reactor(Module("Improved big reactor", 6, 0.0f,
                            {{Resource::Metals, 25.0f}, {Resource::RareMetals, 10.0f}, {Resource::Crystals, 10.0f}},
                            {{Resource::Antimatter, 5.0f}}, Module::Inside),
                     40.0f)},

    {ModuleType::AdvancedSmallReactor, Module::Reactor(Module("Advanced small reactor", 2, 0.0f,
                                                              {{Resource::Metals, 5.0f},
                                                               {Resource::RareMetals, 5.0f},
                                                               {Resource::Crystals, 5.0f},
                                                               {Resource::DarkMatter, 3.0f}},
                                                              {{Resource::Antimatter, 2.0f}}, Module::Inside),
                                                       20.0f)},
    {ModuleType::AdvancedBigReactor, Module::Reactor(Module("Advanced big reactor", 6, 0.0f,
                                                            {{Resource::Metals, 20.0f},
                                                             {Resource::RareMetals, 20.0f},
                                                             {Resource::Crystals, 20.0f},
                                                             {Resource::DarkMatter, 15.0f}},
                                                            {{Resource::Antimatter, 5.0f}}, Module::Inside),
                                                     55.0f)},

    {ModuleType::BasicShieldGenerator,
     Module::ShieldGenerator(Module("Basic shield generator", 1, 1.5f,
                                    {{Resource::Metals, 5.0f}, {Resource::Crystals, 3.0f}}, {}, Module::Inside),
                             100.0f)},
    {ModuleType::ImprovedShieldGenerator,
     Module::ShieldGenerator(Module("Improved shield generator", 1, 3.0f,
                                    {{Resource::RareMetals, 5.0f}, {Resource::Crystals, 10.0f}}, {}, Module::Inside),
                             200.0f)},
    {ModuleType::AdvancedShieldGenerator,
     Module::ShieldGenerator(
         Module("Advanced shield generator", 1, 6.0f,
                {{Resource::Metals, 5.0f}, {Resource::Crystals, 10.0f}, {Resource::DarkMatter, 5.0f}}, {},
                Module::Inside),
         400.0f)},

    {ModuleType::MetalArmorPlates,
     Module::ArmorPlates(Module("Metal armor plates", 2, 0.0f, {{Resource::Metals, 15.0f}}, {}, Module::Sides),
                         100.0f)},
    {ModuleType::ImprovedArmorPlates,
     Module::ArmorPlates(Module("Armor plates enchanced with rare metals", 2, 0.0f,
                                {{Resource::Metals, 15.0f}, {Resource::RareMetals, 10.0f}}, {}, Module::Sides),
                         200.0f)},
    {ModuleType::AdvancedArmorPlates, Module::ArmorPlates(Module("Advanced armor plates", 2, 0.0f,
                                                                 {{Resource::Metals, 15.0f},
                                                                  {Resource::RareMetals, 10.0f},
                                                                  {Resource::Polymers, 10.0f},
                                                                  {Resource::Crystals, 5.0f}},
                                                                 {}, Module::Sides),
                                                          400.0f)},

    {ModuleType::Quarters,
     Module::Quarters(Module("Quarters", 2, 1.0f, {{Resource::Metals, 25.0f}}, {}, Module::Inside), 5.0f)},

    {ModuleType::BasicContructionModule,
     Module::ContructionModule(
         Module("Basic construction module", 4, 4.0f, {{Resource::Metals, 10.0f}}, {}, Module::Sides), 2.0f)},
    {ModuleType::AdvancedContructionModule,
     Module::ContructionModule(
         Module("Advanced construction module", 4, 4.0f,
                {{Resource::Metals, 10.0f}, {Resource::RareMetals, 5.0f}, {Resource::Crystals, 2.0f}}, {},
                Module::Sides),
         4.0f)},
    {ModuleType::NanobotsContructionModule,
     Module::ContructionModule(
         Module("Nanobots construction module", 4, 4.0f,
                {{Resource::Metals, 10.0f}, {Resource::RareMetals, 4.0f}, {Resource::AncientNanobots, 4.0f}}, {},
                Module::Sides),
         8.0f)},

    {ModuleType::NanobotsSelfRepairModule,
     Module("Nanobots self-repair module", 4, 8.0f,
            {{Resource::Metals, 20.0f}, {Resource::RareMetals, 10.0f}, {Resource::AncientNanobots, 20.0f}},
            {{Resource::AncientNanobots, 0.05f}, {Resource::Antimatter, 0.1f}}, Module::Inside)},

    {ModuleType::SmallLaser,
     Module::WeaponModule(
         Module("Small laser", 1, 2.0f, {{Resource::Metals, 5.0f}, {Resource::Crystals, 3.0f}}, {}, Module::Sides),
         Weapon(2, 4.0f, 0.4f, Weapon::ShieldDamageBonus))

    },
    {ModuleType::ImprovedSmallLaser,
     Module::WeaponModule(Module("Advanced small laser", 1, 4.0f,
                                 {{Resource::Metals, 5.0f}, {Resource::RareMetals, 5.0f}, {Resource::Crystals, 6.0f}},
                                 {}, Module::Sides),
                          Weapon(3, 6.0f, 0.4f, Weapon::ShieldDamageBonus))

    },
    {ModuleType::BigLaser,
     Module::WeaponModule(
         Module("Big laser", 4, 10.0f,
                {{Resource::Metals, 20.0f}, {Resource::RareMetals, 20.0f}, {Resource::Crystals, 20.0f}}, {},
                Module::Sides),
         Weapon(1, 90.0f, 1.2f, Weapon::ShieldDamageBonus))

    },
    {ModuleType::AdvancedBigLaser, Module::WeaponModule(Module("Advanced big laser", 4, 15.0f,
                                                               {{Resource::Metals, 20.0f},
                                                                {Resource::RareMetals, 20.0f},
                                                                {Resource::Crystals, 20.0f},
                                                                {Resource::DarkMatter, 2.0f}},
                                                               {}, Module::Sides),
                                                        Weapon(1, 150.0f, 1.5f, Weapon::ShieldDamageBonus))

    },
    {ModuleType::AntimatterCannon,
     Module::WeaponModule(
         Module("Antimatter cannon", 2, 1.0f, {{Resource::Metals, 10.0f}, {Resource::RareMetals, 15.0f}},
                {{Resource::Antimatter, 0.25f}}, Module::Sides),
         Weapon(1, 20.0f, 0.6f, Weapon::HPDamageBonus))

    },
    {ModuleType::Railgun,
     Module::WeaponModule(Module("Railgun", 1, 1.0f, {{Resource::Metals, 5.0f}, {Resource::RareMetals, 3.0f}},
                                 {{Resource::Metals, 0.1f}}, Module::Sides),
                          Weapon(4, 2.0f, 0.6f, Weapon::None))

    },
    {ModuleType::AdvancedRailgun,
     Module::WeaponModule(Module("Advanced railgun", 1, 2.0f,
                                 {{Resource::Metals, 6.0f}, {Resource::RareMetals, 6.0f}, {Resource::Polymers, 3.0f}},
                                 {{Resource::Metals, 0.1f}}, Module::Sides),
                          Weapon(4, 5.0f, 0.7f, Weapon::None))

    },
};
