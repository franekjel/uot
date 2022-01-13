#include "player.h"

Player::Player(const unsigned int id_, const std::shared_ptr<Galaxy> &known_galaxy_,
               const std::map<Resource, float> &owned_resources_, const std::shared_ptr<Colony> &starting_colony)
{
    id = id_;
    known_galaxy = known_galaxy_;
    owned_resources = {};
    resources_changed = {};
    for (const auto &resource : owned_resources_)
    {
        owned_resources[resource.first] = resource.second;
        resources_changed[resource.first] = true;
    }
    owned_colonies = {};
    owned_colonies[starting_colony->id] = starting_colony;
    owned_space_bases = {};
    owned_fleets = {};
    researched_technology = {};
    available_buildings = {};
    DiscoverTechnology(Technology::TechnologyType::Engineering);
    DiscoverTechnology(Technology::TechnologyType::Spaceships);
}

void Player::DiscoverTechnology(Technology::TechnologyType technology)
{
    known_technologies.insert(technology);
    auto tech = Technologies.at(technology);
    for (const auto &t : tech.unlock)
    {
        available_technologies.insert(t);
    }

    for (const auto &b : Buildings)
    {
        const auto &required = b.second.required_technologies;
        if (find(required.begin(), required.end(), technology) != required.end())
        {
            if (required.size() == 1)  // pewnie najczęstszy przypadek, dlatego wyróżniam
                available_buildings.insert(b.first);
            else
            {
                int intersection_count = 0;

                for (const auto &r : required)
                    if (find(known_technologies.begin(), known_technologies.end(), r) != known_technologies.end())
                        intersection_count++;

                if (intersection_count == required.size())  // sprawdzenie czy znamy wszystkie wymagane technologie
                    available_buildings.insert(b.first);
            }
        }
    }
}

void Player::HandleStartTechnologyResearch(Technology::TechnologyType technology)
{
    if (available_technologies.count(technology) > 0 && known_technologies.count(technology) == 0)
        researched_technology = {technology};

    if (technology == Technology::TechnologyType::None)
        researched_technology = {};
}

void Player::HandleBuildRequest(Building::BuildingType type, Building::BuildingType upgrade_from,
                                unsigned int colony_id)
{
    auto building = Colony::GetBuildingFromType(type);
    auto colony = owned_colonies.find(colony_id);
    if (colony == owned_colonies.end())
        return;
    for (const auto &res : building.cost)
    {
        if (owned_resources[res.first] < res.second)
            return;
    }

    for (const auto &res : building.cost)
    {
        owned_resources[res.first] -= res.second;
        resources_changed[res.first] = true;
    }

    colony->second->AddBuildingToQueue(type, upgrade_from);
}

void Player::HandleMoveFleetRequest(int fleet_id, Point position)
{
    owned_fleets[fleet_id]->wanted_position = position;
}
