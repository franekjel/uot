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
    if (starting_colony)
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
    known_galaxy = whole_galaxy_;
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

    for (const auto &b : Modules)
    {
        const auto &required = b.second.required_technologies;
        if (find(required.begin(), required.end(), technology) != required.end())
        {
            if (required.size() == 1)  // pewnie najczęstszy przypadek, dlatego wyróżniam
                available_modules.insert(b.first);
            else
            {
                int intersection_count = 0;

                for (const auto &r : required)
                    if (find(known_technologies.begin(), known_technologies.end(), r) != known_technologies.end())
                        intersection_count++;

                if (intersection_count == required.size())  // sprawdzenie czy znamy wszystkie wymagane technologie
                    available_modules.insert(b.first);
            }
        }
    }

    for (const auto &b : ShipHulls)
    {
        const auto &required = b.second.required_technologies;
        if (find(required.begin(), required.end(), technology) != required.end())
        {
            if (required.size() == 1)  // pewnie najczęstszy przypadek, dlatego wyróżniam
                available_hulls.insert(b.first);
            else
            {
                int intersection_count = 0;

                for (const auto &r : required)
                    if (find(known_technologies.begin(), known_technologies.end(), r) != known_technologies.end())
                        intersection_count++;

                if (intersection_count == required.size())  // sprawdzenie czy znamy wszystkie wymagane technologie
                    available_hulls.insert(b.first);
            }
        }
    }

    for (const auto &[res, bonus] : tech.resources_modifiers_planet)
    {
        if (resources_modifiers_planet.count(res) > 0)
        {
            resources_modifiers_planet[res] += resources_modifiers_planet[res] * bonus;
        }
        else
        {
            resources_modifiers_planet[res] = 1.0f + bonus;
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

    auto av_buildings = colony->second->GetAvailableBuildings();
    if (av_buildings.count(type) == 0 || av_buildings[type] <= 0)
        return;
    if (upgrade_from != Building::BuildingType::None && colony->second->buildings.count(upgrade_from) <= 0 &&
        colony->second->buildings[upgrade_from] <= 0)
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

    for (const auto &[id, ship] : second_fleet->ships)
    {
        ship->fleet = first_fleet;
    }

    first_fleet->ships.merge(second_fleet->ships);

    first_fleet->location_sector->present_fleets.erase(
        first_fleet->location_sector->present_fleets.find(second_fleet_id));

    owned_fleets[first_fleet_id]->location_sector->DecrementWatcher(owned_fleets[first_fleet_id]->owner_id);
    first_fleet->human_capacity += second_fleet->human_capacity;
    first_fleet->soldiers += second_fleet->soldiers;
    first_fleet->civilians += second_fleet->civilians;
    first_fleet->construction_points += second_fleet->construction_points;

    owned_fleets.erase(owned_fleets.find(second_fleet_id));

    auto &sector = owned_fleets[first_fleet_id]->location_sector;
    sector->joined_fleets.push_back(Sector::JoinedFleets{first_fleet_id, second_fleet_id,
                                                         owned_fleets[first_fleet_id]->owner_id,
                                                         Sector::FleetParameters(owned_fleets[first_fleet_id])});
}

void Player::HandleWarpLoadingFleetRequest(unsigned int fleet_id)
{
    if (owned_fleets.count(fleet_id) < 1)
        return;
    auto &current_fleet = owned_fleets[fleet_id];

    if (current_fleet->current_action != Fleet::Action::None)
        return;

    auto &this_sector = current_fleet->location_sector;
    bool is_near_warp_point = false;
    for (const auto &neigh : this_sector->neighbors)
    {
        is_near_warp_point |=
            ((neigh->position - this_sector->position).normalized() - current_fleet->position).squaredLength() <=
            Fleet::kNearValue;
    }

    if (!is_near_warp_point)
        return;
    owned_fleets[fleet_id]->current_action = Fleet::Action::WarpLoading;
}

void Player::HandleBuildAsteroidMineFleetRequest(unsigned int fleet_id)
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
            if (inhabitable && (!inhabitable->base || inhabitable->base->owner->id != id))
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

void Player::HandleCancelFleetRequest(unsigned int fleet_id)
{
    if (owned_fleets.count(fleet_id) < 1)
        return;
    auto current_fleet = owned_fleets[fleet_id];

    switch (current_fleet->current_action)
    {
        case Fleet::Action::WarpLoading:
            for (auto &[id, ship] : current_fleet->ships)
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
            current_fleet->colony_building_object->is_being_colonized = false;
            current_fleet->colony_building_object = nullptr;
            current_fleet->building_progress = 0.0f;
            current_fleet->full_building_progress = 0.0f;
            break;
        case Fleet::Action::Invade:
            current_fleet->invaded_colony = nullptr;
            break;
        default:
            break;
    }
    current_fleet->current_action = Fleet::Action::None;
}

void Player::HandleColonizeFleetRequest(unsigned int fleet_id)
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
            if (planet && !planet->colony && !planet->is_being_colonized)
            {
                handled_fleet->colony_building_object = planet;
                planet->is_being_colonized = true;
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

void Player::HandleShipDesignRequest(unsigned int id, bool delete_design, std::string name, ShipHull::Type hull_type,
                                     std::map<ModuleType, int> sides, std::map<ModuleType, int> inside)
{
    if (delete_design && ship_designs.count(id) > 0)
    {
        changed_designs.push_back({id, nullptr, true});
        ship_designs.erase(id);
        return;
    }

    if (available_hulls.count(hull_type) == 0)
        return;

    for (const auto &side : sides)
    {
        if (available_modules.count(side.first) == 0)
            return;
    }

    for (const auto &ins : inside)
    {
        if (available_modules.count(ins.first) == 0)
            return;
    }

    auto sides_size = ShipHulls.at(hull_type).sides_size;
    auto inside_size = ShipHulls.at(hull_type).inside_size;

    for (const auto &[module_type, count] : sides)
    {
        if (Modules.at(module_type).destination != Module::ModuleDestination::Sides)
        {
            return;
        }

        sides_size -= Modules.at(module_type).size * count;
    }

    for (const auto &[module_type, count] : inside)
    {
        if (Modules.at(module_type).destination != Module::ModuleDestination::Inside)
        {
            return;
        }

        inside_size -= Modules.at(module_type).size * count;
    }

    if (sides_size < 0 || inside_size < 0)
    {
        return;
    }

    if (!IsShipDesignCorrect(hull_type, sides, inside))
        return;

    auto current_design = std::make_shared<ShipDesign>(id, name, hull_type, sides, inside);
    changed_designs.push_back({id, current_design, false});
    ship_designs[id] = current_design;
}

void Player::HandleCreateShipRequest(unsigned int design_id, unsigned int planet_id)
{
    std::shared_ptr<Colony> current_colony;
    std::shared_ptr<Planet> current_planet;
    std::shared_ptr<Sector> current_sector;

    for (const auto &[colony_id, colony] : owned_colonies)
    {
        if (colony->planet->id == planet_id)
        {
            current_colony = colony;
            current_planet = colony->planet;
            current_sector = known_galaxy->sectors[current_planet->sector_id];
            break;
        }
    }

    if (!current_colony || ship_designs.count(design_id) < 1)
    {
        return;
    }

    bool can_build = false;
    Building::BuildingType shipyard_type = Building::BuildingType::None;
    for (const auto &[buld_type, buld] : current_colony->buildings)
    {
        if (buld_type == Building::BuildingType::SmallOrbitalShipyard ||
            buld_type == Building::BuildingType::MediumOrbitalShipyard ||
            buld_type == Building::BuildingType::GrandOrbitalShipyard)
        {
            if (shipyard_type < buld_type)
                shipyard_type = buld_type;
            can_build = true;
            break;
        }
    }

    if (!can_build)
    {
        return;
    }

    if ((int)shipyard_type < (int)ship_designs[design_id]->hull_type)
    {
        return;
    }

    auto &design = ship_designs[design_id];

    for (const auto &[resource, count] : design->cost)
    {
        if (owned_resources[resource] < count)
        {
            can_build = false;
            break;
        }
    }

    if (!can_build)
    {
        return;
    }

    for (const auto &[resource, count] : design->cost)
        owned_resources[resource] -= count;

    current_colony->ship_building_queue.push_back({design, current_sector});
    current_colony->ship_building_queue_changed = true;
}

void Player::HandleHumansMoveFleet(unsigned int fleet_id, Fleet::Action type)
{
    if (owned_fleets.count(fleet_id) < 1)
        return;
    const auto &fleet = owned_fleets[fleet_id];
    std::shared_ptr<Colony> colony = nullptr;
    for (const auto &object : fleet->location_sector->objects)
    {
        if ((object.second->position - fleet->position).squaredLength() < Fleet::kNearValue)
        {
            auto planet = std::dynamic_pointer_cast<Planet>(object.second);
            if (planet && planet->colony && planet->colony->owner->id == id)
            {
                colony = planet->colony;
                break;
            }
        }
    }
    if (!colony)
        return;

    switch (type)
    {
        case Fleet::Action::KickOutCivilians:
            fleet->MoveCiviliansToColony(colony);
            break;
        case Fleet::Action::KidnapCivilians:
            fleet->MoveCiviliansFromColony(colony);
            break;
        case Fleet::Action::DisembarkSoldiers:
            fleet->MoveSoldiersToColony(colony);
            break;
        case Fleet::Action::BorrowSoldiers:
            fleet->MoveSoldiersFromColony(colony);
            break;
        default:
            break;
    }
}

void Player::HandleInvadeFleetRequest(unsigned int fleet_id)
{
    if (owned_fleets.count(fleet_id) < 1)
        return;
    const auto &fleet = owned_fleets[fleet_id];
    std::shared_ptr<Colony> colony = nullptr;
    for (const auto &object : fleet->location_sector->objects)
    {
        if ((object.second->position - fleet->position).squaredLength() < Fleet::kNearValue)
        {
            auto planet = std::dynamic_pointer_cast<Planet>(object.second);
            if (planet && planet->colony && planet->colony->owner->id != id)
            {
                colony = planet->colony;
                break;
            }
        }
    }
    if (!colony)
        return;
    fleet->InvadeColony(colony);
}
