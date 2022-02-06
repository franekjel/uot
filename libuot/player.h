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
    std::map<unsigned int, std::shared_ptr<ShipDesign>> ship_designs;
    std::map<unsigned int, std::shared_ptr<Fleet>> owned_fleets;
    std::map<Resource, float> owned_resources;
    std::map<Resource, float> resources_modifiers_planet;
    std::map<Resource, float> resources_modifiers_inhabitable;
    float population_growth_modifier = 1.0f;

    std::set<Technology::TechnologyType> known_technologies;
    std::set<Technology::TechnologyType> available_technologies;
    std::set<Building::BuildingType> available_buildings;
    TechnologyProgress researched_technology;

    void HandleBuildRequest(Building::BuildingType type, Building::BuildingType upgrade_from, unsigned int colony_id);
    void HandleMoveFleetRequest(int fleet_id, Point position);
    void HandleJoinFleetRequest(int first_fleet_id, int second_fleet_id);
    void HandleWarpLoadingFleetRequest(int fleet_id);
    void HandleBuildAsteroidMineFleetRequest(int fleet_id);
    void HandleStartTechnologyResearch(Technology::TechnologyType technology);
    void DiscoverTechnology(Technology::TechnologyType technology);

    // client-only
    std::shared_ptr<Galaxy> known_galaxy;

    Player(const unsigned int player_id_, const std::shared_ptr<Galaxy>& known_galaxy_,
           const std::map<Resource, float>& owned_resources_, const std::shared_ptr<Colony>& starting_colony);

    // fields below are server-only
    Player(const unsigned int player_id_, const std::shared_ptr<Galaxy>& whole_galaxy_,
           const std::map<Resource, float>& owned_resources_, const unsigned int& starting_sector_id,
           const unsigned int& starting_colony_obj_id);
    std::map<Resource, bool> resources_changed;
    std::set<Technology::TechnologyType> new_technologies;
};

inline bool operator==(const Player& lhs, const Player& rhs) { return lhs.id == rhs.id; }

inline bool operator!=(const Player& lhs, const Player& rhs) { return !(lhs == rhs); }
