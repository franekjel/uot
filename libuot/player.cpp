#include "player.h"

Player::Player(const long player_id_, const std::shared_ptr<Galaxy> &known_galaxy_,
               const std::map<Resource, float> &owned_resources_, const std::shared_ptr<Colony> &starting_colony)
{
    player_id = player_id_;
    known_galaxy = known_galaxy_;
    owned_resources = {};
    for (const auto &resource : owned_resources_)
    {
        owned_resources[resource.first] = resource.second;
    }
    owned_colonies = {};
    owned_colonies.push_back(starting_colony);
    owned_ships = {};
    DiscoverTechnology(&Engineering);
}

void Player::DiscoverTechnology(const Technology *const technology)
{
    known_technologies.insert(technology);
    for (const auto &t : technology->unlock)
    {
        available_technologies.insert(t);
    }
}
