#include "messagestructs.h"

messageTypes::MsgStar::MsgStar() {}
messageTypes::MsgStar::MsgStar(const std::shared_ptr<Star>& star)
    : id(star->id), position(star->position), object_size(star->size), starType(star->star_type)
{
}

messageTypes::MsgBase::MsgBase() {}
messageTypes::MsgBase::MsgBase(const std::shared_ptr<SpaceBase>& spaceBase)
    : id(spaceBase->id), owner_id(spaceBase->owner->id)
{
}

messageTypes::MsgInhabitable::MsgInhabitable()
{
    resurce_deposit = {};
    base = {};
    base_exists = false;
}
messageTypes::MsgInhabitable::MsgInhabitable(const std::shared_ptr<InhabitableObject>& inhabitable)
    : id(inhabitable->id),
      position(inhabitable->position),
      object_size(inhabitable->size),
      object_type(inhabitable->object_type)
{
    for (auto resource : inhabitable->resource_deposit)
        resurce_deposit[resource.first] = resource.second;

    if (!!inhabitable->base)
    {
        base = MsgBase(inhabitable->base);
        base_exists = true;
    }
    else
        base_exists = false;
}

messageTypes::MsgColony::MsgColony() { buildings = {}; }
messageTypes::MsgColony::MsgColony(const std::shared_ptr<Colony>& colony)
    : id(colony->id), population(colony->population), owner_id(colony->owner->id)
{
    for (auto building : colony->buildings)
        buildings[building.first] = building.second;
}

messageTypes::MsgPlanet::MsgPlanet()
{
    planetary_features = {};
    possible_buildings = {};
    colony_exists = false;
    colony = {};
}
messageTypes::MsgPlanet::MsgPlanet(const std::shared_ptr<Planet>& planet)
    : id(planet->id),
      position(planet->position),
      object_size(planet->SectorObject::size),
      climate(planet->climate),
      planet_size(planet->Planet::size)
{
    if (!!planet->colony)
    {
        colony = MsgColony(planet->colony);
        colony_exists = true;
    }
    else
        colony_exists = false;

    for (auto feature : planet->planetary_features)
        planetary_features.insert(feature);

    for (auto building : planet->possible_buildings)
        possible_buildings[building.first] = building.second;
}

messageTypes::MsgNeighbor::MsgNeighbor() {}
messageTypes::MsgNeighbor::MsgNeighbor(const std::shared_ptr<Sector>& sector)
    : id(sector->sector_id), position(sector->position)
{
}

messageTypes::MsgSector::MsgSector()
{
    neighbors = {};
    stars = {};
    inhabitables = {};
    planets = {};
}
messageTypes::MsgSector::MsgSector(const std::shared_ptr<Sector>& sector)
    : id(sector->sector_id), position(sector->position)
{
    for (auto neighbor : sector->neighbors)
        neighbors.push_back(MsgNeighbor(neighbor));
    for (auto& [id, sector_object] : sector->objects)
    {
        std::shared_ptr<Star> star = std::dynamic_pointer_cast<Star>(sector_object);
        std::shared_ptr<InhabitableObject> inhabitable = std::dynamic_pointer_cast<InhabitableObject>(sector_object);
        std::shared_ptr<Planet> planet = std::dynamic_pointer_cast<Planet>(sector_object);
        if (!!star)
            stars.push_back(MsgStar(star));
        if (!!inhabitable)
            inhabitables.push_back(MsgInhabitable(inhabitable));
        if (!!planet)
            planets.push_back(MsgPlanet(planet));
    }
}

messageTypes::MsgGalaxy::MsgGalaxy() { sectors = {}; }
messageTypes::MsgGalaxy::MsgGalaxy(const std::shared_ptr<Galaxy>& galaxy, const std::shared_ptr<Player>& player)
{
    auto sector = galaxy->sectors[player->owned_colonies.begin()->second->planet->sector_id];
    sectors.push_back(MsgSector(sector));
}

messageTypes::MsgTechnologyUpdate::MsgTechnologyUpdate() {}

messageTypes::MsgTechnologyUpdate::MsgTechnologyUpdate(Technology::TechnologyType technology_type_, int days_remaining_)
    : technology_type(technology_type_), days_remaining(days_remaining_)
{
}

messageTypes::MsgBuildingsUpdates::MsgBuildingsUpdates() {}

messageTypes::MsgBuildingsUpdates::MsgBuildingsUpdates(int colony_id_, Building::BuildingType building_type_,
                                                       Building::BuildingType upgrade_of_, int days_remaining_)
    : colony_id(colony_id_), building_type(building_type_), upgrade_of(upgrade_of_), days_remaining(days_remaining_)
{
}

messageTypes::MsgBuildRequest::MsgBuildRequest() {}

messageTypes::MsgBuildRequest::MsgBuildRequest(int colony_id_, Building::BuildingType building_type_,
                                               Building::BuildingType upgrade_from_)
    : colony_id(colony_id_), building_type(building_type_), upgrade_from(upgrade_from_)
{
}

messageTypes::MsgMoveFleetRequest::MsgMoveFleetRequest() {}

messageTypes::MsgMoveFleetRequest::MsgMoveFleetRequest(int fleet_id_, Point position_)
    : fleet_id(fleet_id_), position(position_)
{
}

messageTypes::MsgFleet::MsgFleet() {}

messageTypes::MsgFleet::MsgFleet(const std::shared_ptr<Fleet>& fleet, unsigned int player_id_) : player_id(player_id_)
{
    id = fleet->id;
    position = fleet->position;
}

messageTypes::MsgFleetsJoin::MsgFleetsJoin() {}

messageTypes::MsgFleetsJoin::MsgFleetsJoin(const Sector::JoinedFleets& joined_fleets)
    : joined_fleet_id_1(joined_fleets.joined_fleet_1),
      joined_fleet_id_2(joined_fleets.joined_fleet_2),
      result_fleet_id(joined_fleets.res_fleet),
      result_fleet_pos(joined_fleets.res_fleet_pos)
{
}

messageTypes::MsgFleetsJump::MsgFleetsJump() {}

messageTypes::MsgFleetsJump::MsgFleetsJump(const Sector::JumpedFleet& jumped_fleets)
    : fleet_id(jumped_fleets.fleet_id),
      sector_id_from(jumped_fleets.sector_id_from),
      sector_id_to(jumped_fleets.sector_id_to),
      position(jumped_fleets.position)
{
}

messageTypes::MsgNewBase::MsgNewBase() {}

messageTypes::MsgNewBase::MsgNewBase(const Sector::NewBase& new_base)
    : base_id(new_base.base_id), object_id(new_base.object_id), owner(new_base.owner)
{
}

messageTypes::MsgNewColony::MsgNewColony() {}

messageTypes::MsgNewColony::MsgNewColony(const Sector::NewColony& new_colony)
    : colony_id(new_colony.colony_id),
      object_id(new_colony.object_id),
      owner(new_colony.owner),
      population(new_colony.population)
{
}

messageTypes::MsgWatchedSectorUpdate::MsgWatchedSectorUpdate() {}

messageTypes::MsgWatchedSectorUpdate::MsgWatchedSectorUpdate(int sector_id_) : sector_id(sector_id_) { fleets = {}; }

messageTypes::MsgJoinFleetsRequest::MsgJoinFleetsRequest() {}

messageTypes::MsgJoinFleetsRequest::MsgJoinFleetsRequest(int fleet1_id_, int fleet2_id_)
    : fleet1_id(fleet1_id_), fleet2_id(fleet2_id_)
{
}

messageTypes::MsgFleetActionRequest::MsgFleetActionRequest() {}

messageTypes::MsgFleetActionRequest::MsgFleetActionRequest(int fleet_id_, Fleet::Action action_)
    : fleet_id(fleet_id_), action(action_)
{
}

messageTypes::MsgShipDesign::MsgShipDesign() {}

messageTypes::MsgShipDesign::MsgShipDesign(const std::shared_ptr<ShipDesign>& design, bool delete_design_)
    : delete_design(delete_design_)
{
    id = design->id;
    name = design->name;
    hull_type = design->hull_type;
    sides = design->sides;
    inside = design->inside;
}

messageTypes::MsgCreateShip::MsgCreateShip() {}

messageTypes::MsgCreateShip::MsgCreateShip(unsigned int design_id_, unsigned int planet_id_)
    : design_id(design_id_), planet_id(planet_id_)
{
}

messageTypes::MsgShipDesignResponse::MsgShipDesignResponse() {}

messageTypes::MsgShipDesignResponse::MsgShipDesignResponse(unsigned int design_id_,
                                                           const std::shared_ptr<ShipDesign>& design, bool deleted_)
    : deleted(deleted_)
{
    id = design_id_;
    if (!deleted)
    {
        name = design->name;
        hull_type = design->hull_type;
        sides = design->sides;
        inside = design->inside;
        cost = design->cost;
        upkeep = design->upkeep;
        worker_weeks_cost = design->worker_weeks_cost;
    }
}

messageTypes::MsgCreateShipResponse::MsgCreateShipResponse() {}

messageTypes::MsgCreateShipResponse::MsgCreateShipResponse(unsigned int design_id_, unsigned int planet_id_,
                                                           bool created_, bool new_fleet, std::shared_ptr<Ship> ship)
    : design_id(design_id_), planet_id(planet_id_), created(created_)
{
    if (created)
    {
        id = ship->id;
        fleet_parameters = MsgFleetParameters(ship, new_fleet);
    }
    else
    {
        id = 0;
    }
}

messageTypes::MsgFleetParameters::MsgFleetParameters() {}

messageTypes::MsgFleetParameters::MsgFleetParameters(std::shared_ptr<Ship> ship, bool new_fleet_)
    : new_fleet(new_fleet_)
{
    auto& fleet = ship->fleet;

    id = fleet->id;
}
