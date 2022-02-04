#pragma once

#include <map>
#include <vector>

#include "galaxy.h"
#include "planet.h"
#include "resource.h"
#include "ship.h"
#include "spacebase.h"
#include "technology.h"

struct Player
{
    unsigned int id;

    std::map<unsigned int, std::shared_ptr<Colony>> owned_colonies;
    std::map<unsigned int, std::shared_ptr<SpaceBase>> owned_space_bases;
    std::shared_ptr<Galaxy> known_galaxy;
    std::map<unsigned int, std::shared_ptr<Fleet>> owned_fleets;
    std::map<Resource, float> owned_resources;
    std::map<Resource, float> resources_modifiers_planet;
    std::map<Resource, float> resources_modifiers_inhabitable;
    float population_growth_modifier = 1.0f;

    std::set<Technology::TechnologyType> known_technologies;
    std::set<Technology::TechnologyType> available_technologies;
    std::set<Building::BuildingType> available_buildings;
    TechnologyProgress researched_technology;

    Player(const unsigned int player_id_, const std::shared_ptr<Galaxy>& known_galaxy_,
           const std::map<Resource, float>& owned_resources_, const std::shared_ptr<Colony>& starting_colony);

    void HandleBuildRequest(Building::BuildingType type, Building::BuildingType upgrade_from, unsigned int colony_id);
    void HandleMoveFleetRequest(int fleet_id, Point position);
    void HandleStartTechnologyResearch(Technology::TechnologyType technology);
    void DiscoverTechnology(Technology::TechnologyType technology);

    // fields below are server-only
    std::map<Resource, bool> resources_changed;
    std::set<Technology::TechnologyType> new_technologies;
};

inline bool operator==(const Player& lhs, const Player& rhs) { return lhs.id == rhs.id; }

inline bool operator!=(const Player& lhs, const Player& rhs) { return !(lhs == rhs); }
