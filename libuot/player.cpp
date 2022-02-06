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
    if (owned_fleets.count(fleet_id) < 1)
        return;
    if (owned_fleets[fleet_id]->current_action != Fleet::Action::None)
        return;
    owned_fleets[fleet_id]->wanted_position = position;
}

void Player::HandleJoinFleetRequest(unsigned int first_fleet_id, unsigned int second_fleet_id)
{
    if (owned_fleets.count(first_fleet_id) < 1 || owned_fleets.count(second_fleet_id) < 1)
        return;

    auto first_fleet = owned_fleets[first_fleet_id];
    auto second_fleet = owned_fleets[second_fleet_id];

    if (first_fleet->location_sector->sector_id != second_fleet->location_sector->sector_id)
        return;

    if ((first_fleet->position - second_fleet->position).squaredLength() > Fleet::kNearValue)
        return;

    first_fleet->ships.insert(first_fleet->ships.end(), second_fleet->ships.begin(), second_fleet->ships.end());

    first_fleet->location_sector->present_fleets.erase(
        first_fleet->location_sector->present_fleets.find(second_fleet_id));

    owned_fleets[first_fleet_id]->location_sector->DecrementWatcher(owned_fleets[first_fleet_id]->owner_id);
    first_fleet->human_capacity += second_fleet->human_capacity;
    first_fleet->soldiers += second_fleet->soldiers;
    first_fleet->civilians += second_fleet->civilians;
    first_fleet->construction_points += second_fleet->construction_points;

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

void Player::HandleBuildAsteroidMineFleetRequest(int fleet_id)
{
    if (owned_fleets.count(fleet_id) < 1)
        return;

    auto handled_fleet = owned_fleets[fleet_id];

    if (handled_fleet->current_action != Fleet::Action::None || handled_fleet->construction_points <= 0.0f)
        return;

    handled_fleet->base_building_object = nullptr;

    for (const auto &[obj_id, obj] : handled_fleet->location_sector->objects)
    {
        if ((handled_fleet->position - obj->position).squaredLength() <= Fleet::kNearValue)
        {
            auto inhabitable = std::dynamic_pointer_cast<InhabitableObject>(obj);
            if (inhabitable && !inhabitable->base)
            {
                handled_fleet->base_building_object = inhabitable;
                break;
            }
        }
    }
    if (!handled_fleet->base_building_object)
        return;

    auto [labour_cost, base_cost] = handled_fleet->base_building_object->CalcBaseCost();

    bool enough_resources = true;
    for (const auto &[res, count] : base_cost)
    {
        if (owned_resources[res] < count)
        {
            enough_resources = false;
            break;
        }
    }

    if (!enough_resources)
    {
        handled_fleet->base_building_object = nullptr;
        return;
    }

    for (const auto &[res, count] : base_cost)
    {
        owned_resources[res] -= count;
    }

    handled_fleet->full_building_progress = labour_cost;
    handled_fleet->building_progress = 0.0f;
    handled_fleet->current_action = Fleet::Action::BuildAsteroidMine;
}

void Player::HandleCancelFleetRequest(int fleet_id)
{
    if (owned_fleets.count(fleet_id) < 1)
        return;
    auto current_fleet = owned_fleets[fleet_id];

    switch (current_fleet->current_action)
    {
        case Fleet::Action::WarpLoading:
            for (auto &ship : current_fleet->ships)
            {
                ship->warp_drive_charge = 0.0f;
            }
            break;
        case Fleet::Action::BuildAsteroidMine:
            current_fleet->base_building_object = nullptr;
            current_fleet->building_progress = 0.0f;
            current_fleet->full_building_progress = 0.0f;
            break;
        case Fleet::Action::Colonize:
            current_fleet->colony_building_object = nullptr;
            current_fleet->building_progress = 0.0f;
            current_fleet->full_building_progress = 0.0f;
            break;
        case Fleet::Action::Invade:
            // TODO
            break;
        default:
            break;
    }
    current_fleet->current_action = Fleet::Action::None;
}

void Player::HandleColonizeFleetRequest(int fleet_id)
{
    if (owned_fleets.count(fleet_id) < 1)
        return;

    auto handled_fleet = owned_fleets[fleet_id];

    if (handled_fleet->current_action != Fleet::Action::None || handled_fleet->construction_points <= 0.0f ||
        handled_fleet->civilians < Fleet::kColonizationCost)
        return;

    handled_fleet->colony_building_object = nullptr;

    for (const auto &[obj_id, obj] : handled_fleet->location_sector->objects)
    {
        if ((handled_fleet->position - obj->position).squaredLength() <= Fleet::kNearValue)
        {
            auto planet = std::dynamic_pointer_cast<Planet>(obj);
            if (planet && !planet->colony)
            {
                handled_fleet->colony_building_object = planet;
                break;
            }
        }
    }
    if (!handled_fleet->colony_building_object)
        return;

    auto labour_cost = handled_fleet->colony_building_object->colony_work_cost;

    if (owned_resources[Resource::Metals] < handled_fleet->colony_building_object->colony_metal_cost)
    {
        handled_fleet->colony_building_object = nullptr;
        return;
    }
    else
    {
        owned_resources[Resource::Metals] -= handled_fleet->colony_building_object->colony_metal_cost;
        handled_fleet->full_building_progress = labour_cost;
        handled_fleet->building_progress = 0.0f;
        handled_fleet->civilians -= handled_fleet->kColonizationCost;
        handled_fleet->current_action = Fleet::Action::Colonize;
    }
}
