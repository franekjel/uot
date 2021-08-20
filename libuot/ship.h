#pragma once
#include <map>
#include <memory>
#include <set>
#include <vector>

struct Resource;  // when done include relevant header

// TODO These modules (like SmallReactor etc.) should be hold in some global set when we will be doing usable version
struct Module
{
    std::string name;
    int size;
    float energy_usage;
    std::map<Resource, float> cost;

    enum ModuleDestination
    {
        None = 0,
        Front = 1,
        Sides = 2,
        Back = 4,
        Inside = 8,
        All = 15,
    };
    int destination;

    Module(const std::string& name, const int size, const float energy_usage, const std::map<Resource, float>& cost,
           const int destination)
        : name(name), size(size), energy_usage(energy_usage), cost(cost), destination(destination)
    {
    }
};

struct Reactor : Module
{
    float generating_energy;

    Reactor(const Module& m, const float generating_energy) : Module(m), generating_energy(generating_energy) {}
};

const Reactor SmallReactor(Module("Small reactor", 2, 0.0f, {/*TODO: resources*/}, Module::Inside), 10.0f);
const Reactor MediumReactor(Module("Medium reactor", 6, 0.0f, {/*TODO: resources*/}, Module::Inside), 40.0f);
const Reactor ImprovedSmallReactor(Module("Improved small reactor", 2, 0.0f, {/*TODO: resources*/}, Module::Inside),
                                   15.0f);

struct ShieldGenerator : Module
{
    float shield_capacity;
    ShieldGenerator(const Module& m, const float shield_capacity) : Module(m), shield_capacity(shield_capacity) {}
};
const ShieldGenerator SmallShieldGenerator(Module("Small shield generator", 2, 4.0f, {/*TODO: resources*/},
                                                  Module::Inside),
                                           100.0f);

struct ArmorPlates : Module
{
    float additional_hp;
    ArmorPlates(const Module& m, const float additional_hp) : Module(m), additional_hp(additional_hp) {}
};
const ArmorPlates SmallArmorPlates(Module("Small armor plates", 2, 0.0f, {/*TODO: resources*/},
                                          Module::Front | Module::Sides | Module::Back),
                                   100.0f);

struct Engines : Module
{
    float power;
    Engines(const Module& m, const float power) : Module(m), power(power) {}
};
const Engines SmallEngines(Module("Small engines", 2, 2.0f, {/*TODO: resources*/}, Module::Back), 100.0f);

struct Quarters : Module
{
    static constexpr float capacity = 10;
    Quarters() : Module("Cabins", 4, 1.0f, {/*TODO: resources*/}, Module::All) {}
} const Quarters;  // technology cannot make more people live on the same space so there is only one Quarters object

struct Weapon
{
    int attack_count;
    float damage;
    float accurracy;
    float critical_chance;
    float range;

    enum SpecialFeatures
    {
        None = 0,
        BypassShield = 1,
        ShieldDamageBonus = 2,
        ArmorDamageBonus = 4,
    };
    int special_features;

    Weapon(const int attack_count, const float damage, const float accurracy, const float critical_chance,
           const float range, const int special_features)
        : attack_count(attack_count),
          damage(damage),
          accurracy(accurracy),
          critical_chance(critical_chance),
          range(range),
          special_features(special_features)
    {
    }
};

struct WeaponModule : Module, Weapon
{
    WeaponModule(const Module& m, const Weapon& w) : Module(m), Weapon(w) {}
    // TODO: Maybe some variable like color - to display some kind of colorful lasers
};

const WeaponModule SmallLaser1Module(Module("Small laser", 1, 2.0f, {/*TODO: resources*/},
                                            Module::Front | Module::Sides | Module::Back),
                                     Weapon(5, 4.0f, 0.8f, 0.05f, 20.0f, Weapon::ArmorDamageBonus));
const WeaponModule SmallLaser2Module(Module("Improved small laser", 1, 2.0f, {/*TODO: resources*/},
                                            Module::Front | Module::Sides | Module::Back),
                                     Weapon(6, 6.0f, 0.85f, 0.05f, 25.0f, Weapon::ArmorDamageBonus));

// TODO later: special modules like hp regeneration -

struct ShipHull
{
    int front_size;
    int sides_size;
    int back_size;
    int inside_size;
    std::map<Resource, float> cost;

    ShipHull(const int front_size, const int sides_size, const int back_size, const int inside_size,
             const std::map<Resource, float>& cost)
        : front_size(front_size), sides_size(sides_size), back_size(back_size), inside_size(inside_size), cost(cost)
    {
    }
};

const ShipHull SmallShipHull(2, 4, 1, 4, {/*TODO: resources*/});
const ShipHull MediumShipHull(6, 12, 4, 10, {/*TODO: resources*/});

struct ShipDesign
{
    std::string name;
    ShipHull hull;
    std::set<Module> front;
    std::set<Module> sides;
    std::set<Module> back;
    std::set<Module> inside;

    ShipDesign(const std::string& name, const ShipHull& hull, const std::set<Module>& front,
               const std::set<Module>& sides, const std::set<Module>& back, const std::set<Module>& inside)
        : name(name), hull(hull), front(front), sides(sides), back(back), inside(inside)
    {
    }
};

struct Ship
{
    float hp;
    float max_hp;
    float hp_regen;
    float shield;
    float max_shield;
    static constexpr float shield_regen_percentage = 0.01;  // shield regenerating ratio
    static constexpr float shield_regen_energy_cost = 0.1;  // energy cost per shield point
    float energy;
    float energy_regen;
    float max_energy;
    float soldiers;
    float civilians;
    float human_capacity;
    float engines_power;
    float engines_energy_consumtion;
    std::vector<Weapon> weapons;
    std::shared_ptr<ShipDesign> design;
};
