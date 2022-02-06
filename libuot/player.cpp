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

Player::Player(const unsigned int player_id_, const std::shared_ptr<Galaxy> &whole_galaxy_,
               const std::map<Resource, float> &owned_resources_, const unsigned int &starting_sector_id,
               const unsigned int &starting_colony_obj_id)
{
    id = player_id_;
    owned_resources = {};
    resources_changed = {};
    for (const auto &resource : owned_resources_)
    {
        owned_resources[resource.first] = resource.second;
        resources_changed[resource.first] = true;
    }
    owned_colonies = {};
    auto &starting_sector = whole_galaxy_->sectors[starting_sector_id];
    auto &obj = starting_sector->objects[starting_colony_obj_id];
    std::shared_ptr<Planet> starting_planet = std::dynamic_pointer_cast<Planet>(obj);
    owned_colonies[starting_planet->colony->id] = starting_planet->colony;
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
    new_technologies.insert(technology);
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

    for (const auto &bonus : tech.resources_modifiers_planet)
    {
        if (resources_modifiers_planet.count(bonus.first) > 0)
        {
            resources_modifiers_planet[bonus.first] += resources_modifiers_planet[bonus.first] * bonus.second;
        }
        else
        {
            resources_modifiers_planet[bonus.first] = 1.0f + bonus.second;
        }
    }

    for (const auto &bonus : tech.resources_modifiers_inhabitable)
    {
        if (resources_modifiers_inhabitable.count(bonus.first) > 0)
        {
            resources_modifiers_inhabitable[bonus.first] += resources_modifiers_inhabitable[bonus.first] * bonus.second;
        }
        else
        {
            resources_modifiers_inhabitable[bonus.first] = 1.0f + bonus.second;
        }
    }

    population_growth_modifier += tech.population_growth_modifier * population_growth_modifier;
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

void Player::HandleMoveFleetRequest(unsigned int fleet_id, Point position)
{
    owned_fleets[fleet_id]->wanted_position = position;
}

void Player::HandleJoinFleetRequest(unsigned int first_fleet_id, unsigned int second_fleet_id)
{
    if (owned_fleets.count(first_fleet_id) < 1 || owned_fleets.count(second_fleet_id) < 1)
        return;

    if (owned_fleets[first_fleet_id]->location_sector->sector_id !=
        owned_fleets[second_fleet_id]->location_sector->sector_id)
        return;

    if ((owned_fleets[first_fleet_id]->position - owned_fleets[second_fleet_id]->position).squaredLength() >
        Fleet::kNearValue)
        return;

    owned_fleets[first_fleet_id]->ships.insert(owned_fleets[first_fleet_id]->ships.end(),
                                               owned_fleets[second_fleet_id]->ships.begin(),
                                               owned_fleets[second_fleet_id]->ships.end());

    owned_fleets[first_fleet_id]->location_sector->present_fleets.erase(
        owned_fleets[first_fleet_id]->location_sector->present_fleets.find(second_fleet_id));

    owned_fleets[first_fleet_id]->location_sector->DecrementWatcher(owned_fleets[first_fleet_id]->owner_id);

    owned_fleets.erase(owned_fleets.find(second_fleet_id));

    auto &sector = owned_fleets[first_fleet_id]->location_sector;
    sector->joined_fleets.push_back(Sector::JoinedFleets{first_fleet_id, second_fleet_id, first_fleet_id,
                                                         owned_fleets[first_fleet_id]->position,
                                                         owned_fleets[first_fleet_id]->owner_id});
}

void Player::HandleWarpLoadingFleetRequest(int fleet_id)
{
    if (owned_fleets.count(fleet_id) < 1)
        return;
    if (owned_fleets[fleet_id]->current_action != Fleet::Action::None)
        return;
    if (std::abs(owned_fleets[fleet_id]->position.squaredLength() - 1.0f) > Fleet::kNearValue)
        return;
    owned_fleets[fleet_id]->current_action = Fleet::Action::WarpLoading;
}
