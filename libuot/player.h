#pragma once

#include <map>
#include <vector>

#include "galaxy.h"
#include "planet.h"
#include "resource.h"
#include "ship.h"
#include "technology.h"
#include "spacebase.h"

struct Player
{
    unsigned int id;

    std::vector<std::shared_ptr<Colony>> owned_colonies;
    std::vector<std::shared_ptr<SpaceBase>> owned_space_bases;
    std::shared_ptr<Galaxy> known_galaxy;
    std::vector<std::shared_ptr<Ship>> owned_ships;
    std::map<Resource, float> owned_resources;
    std::map<Resource, bool> resources_changed;

    std::set<const Technology*> known_technologies;
    std::set<const Technology*> available_technologies;

    Player(const long player_id_, const std::shared_ptr<Galaxy>& known_galaxy_,
           const std::map<Resource, float>& owned_resources_, const std::shared_ptr<Colony>& starting_colony);

    void DiscoverTechnology(const Technology* const technology);
};

inline bool operator==(const Player& lhs, const Player& rhs) { return lhs.id == rhs.id; }

inline bool operator!=(const Player& lhs, const Player& rhs) { return !(lhs == rhs); }
