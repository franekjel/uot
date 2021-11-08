#pragma once

#include <map>
#include <memory>
#include <set>

#include "resource.h"
#include "sectorobject.h"

struct Building
{
    const std::map<Resource, float> cost;
    const std::map<Resource, float> upkeep;
    const float workers;
    const std::map<Resource, float> production;

    const Building* upgrade;

    bool operator<(const Building& b) const  // whatever, needed to make sets and maps
    {
        return this->workers < b.workers;
    }
};

const Building PlanetaryAdministration{{{Resource::Metals, 50.0f}},
                                       {{Resource::Antimatter, 1.0f}, {Resource::Cryptocurrencies, 10.0f}},
                                       10.0f,
                                       {},
                                       nullptr};

const Building ImprovedMetalsMine{{{Resource::Metals, 50.0f}, {Resource::Crystals, 10.0f}},
                                  {{Resource::Antimatter, 2.0f}, {Resource::Cryptocurrencies, 15.0f}},
                                  15.0f,
                                  {{Resource::Metals, 20.0f}},
                                  nullptr};

const Building MetalsMine{{{Resource::Metals, 50.0f}},
                          {{Resource::Antimatter, 1.0f}, {Resource::Cryptocurrencies, 10.0f}},
                          10.0f,
                          {{Resource::Metals, 10.0f}},
                          &ImprovedMetalsMine};

const Building Farm{{{Resource::Metals, 30.0f}},
                    {{Resource::Antimatter, 1.0f}, {Resource::Cryptocurrencies, 10.0f}},
                    10.0f,
                    {{Resource::Food, 10.0f}},
                    &ImprovedMetalsMine};

const Building Greenhouses{{{Resource::Metals, 50.0f}},
                           {{Resource::Antimatter, 3.0f}, {Resource::Cryptocurrencies, 12.0f}},
                           10.0f,
                           {{Resource::Food, 7.0f}},
                           nullptr};

// buildable buildings here
const std::set<Building> LimitedBuildings = {Farm, MetalsMine};
const std::set<Building> UnlimitedBuildings = {Greenhouses};

struct PlanetaryFeatures
{
    const std::map<Building, int> feature_buildings;
};

const PlanetaryFeatures TemperateClimate = {{{Farm, 6}}};
const PlanetaryFeatures HotClimate = {{{Farm, 2}}};
const PlanetaryFeatures ColdClimate = {{{Farm, 2}}};
const PlanetaryFeatures SmallMetalsDeposits = {{{MetalsMine, 2}}};
const PlanetaryFeatures MediumMetalsDeposits = {{{MetalsMine, 4}}};
const PlanetaryFeatures BigMetalsDeposits = {{{MetalsMine, 8}}};

struct Colony;

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
    std::map<Building, int> possible_buildings;
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

struct Colony
{
    std::shared_ptr<Planet> planet;
    std::map<Building, int> buildings;
    float population;
    long owner_id = 0;

    Colony(const long& owner_id_, const std::shared_ptr<Planet> planet_)
    {
        owner_id = owner_id_;
        population = 1;
        buildings = {};
        planet = planet_;
    }
};
