#pragma once

#include <map>
#include <vector>

#include "galaxy.h"
#include "planet.h"
#include "resource.h"
#include "ship.h"

struct Player
{
    long player_id;

    std::vector<std::shared_ptr<Colony>> owned_colonies;
    std::shared_ptr<Galaxy> known_galaxy;
    std::vector<std::shared_ptr<Ship>> owned_ships;
    std::map<Resource, float> owned_resources;

    Player(const long player_id_, const std::shared_ptr<Galaxy>& known_galaxy_,
           const std::map<Resource, float>& owned_resources_, const std::shared_ptr<Colony>& starting_colony)
    { 
        player_id = player_id_;
        known_galaxy = known_galaxy_;
        owned_resources = {};
        for (auto& resource : owned_resources_)
        {
            owned_resources[resource.first] = resource.second;
        }
        owned_colonies = {};
        owned_colonies.push_back(starting_colony);
        owned_ships = {};

    }
};

inline bool operator==(const Player& lhs, const Player& rhs)
{ 
    return lhs.player_id == rhs.player_id;
}

inline bool operator!=(const Player& lhs, const Player& rhs) { return !(lhs == rhs); }