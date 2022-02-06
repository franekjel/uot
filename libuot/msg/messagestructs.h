#pragma once
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include "planet.h"
#include "player.h"
#include "sector.h"
#include "spacebase.h"

namespace messageTypes
{
struct MsgStar
{
    unsigned int id;
    Point position;
    float object_size;
    Star::StarType starType;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MsgStar, id, position, object_size, starType)
    MsgStar();
    MsgStar(const std::shared_ptr<Star>& star);
};

struct MsgBase
{
    unsigned int id;
    unsigned int owner_id;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MsgBase, id, owner_id)
    MsgBase();
    MsgBase(const std::shared_ptr<SpaceBase>& spaceBase);
};

struct MsgInhabitable
{
    unsigned int id;
    Point position;
    float object_size;
    std::map<Resource, float> resurce_deposit;
    InhabitableObject::ObjectType object_type;
    bool base_exists;
    MsgBase base;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MsgInhabitable, id, position, object_size, resurce_deposit, object_type, base_exists,
                                   base)
    MsgInhabitable();
    MsgInhabitable(const std::shared_ptr<InhabitableObject>& inhabitable);
};

struct MsgColony
{
    unsigned int id;
    std::map<Building::BuildingType, int> buildings;
    float population;
    int owner_id;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MsgColony, id, buildings, population, owner_id)
    MsgColony();
    MsgColony(const std::shared_ptr<Colony>& colony);
};

struct MsgPlanet
{
    unsigned int id;
    Point position;
    float object_size;
    Planet::PlanetClimate climate;
    int planet_size;
    std::map<PlanetaryFeatures::PlanetaryFeatureType, int> planetary_features;
    std::map<Building::BuildingType, int> possible_buildings;
    bool colony_exists;
    MsgColony colony;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MsgPlanet, id, position, object_size, climate, planet_size, planetary_features,
                                   possible_buildings, colony_exists, colony)
    MsgPlanet();
    MsgPlanet(const std::shared_ptr<Planet>& planet);
};

struct MsgSector
{
    int id;
    Point position;
    std::vector<int> neighbors_ids;
    std::vector<MsgStar> stars;
    std::vector<MsgInhabitable> inhabitables;
    std::vector<MsgPlanet> planets;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MsgSector, id, position, neighbors_ids, stars, inhabitables, planets)
    MsgSector();
    MsgSector(const std::shared_ptr<Sector>& sector, bool as_neighbor);
};

struct MsgGalaxy
{
    std::vector<MsgSector> sectors;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MsgGalaxy, sectors)
    MsgGalaxy();
    MsgGalaxy(const std::shared_ptr<Galaxy>& galaxy, const std::shared_ptr<Player>& player);
};

struct MsgShipDesign
{
    unsigned int id;
    std::string name;
    ShipHull::Type hull_type;
    std::map<ModuleType, int> sides;
    std::map<ModuleType, int> inside;
    std::map<Resource, float> cost;    // sum of costs of modules and hull
    std::map<Resource, float> upkeep;  // total upkeep
    float worker_weeks_cost;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MsgShipDesign, id, name, hull_type, sides, inside, cost, upkeep, worker_weeks_cost)
    MsgShipDesign();
    MsgShipDesign(const std::shared_ptr<ShipDesign>& design);
};

struct MsgTechnologyUpdate
{
    Technology::TechnologyType technology_type;  // if none in progress, set to None
    int days_remaining;                          // days reamaining to research end, 0 means end of research
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MsgTechnologyUpdate, technology_type, days_remaining)
    MsgTechnologyUpdate();
    MsgTechnologyUpdate(Technology::TechnologyType technology_type_, int days_remaining_);
};

struct MsgBuildingsUpdates
{
    int colony_id;
    Building::BuildingType building_type;
    Building::BuildingType upgrade_of;  // set if new building is upgrade of another, otherwise, set to None
    int days_remaining;                 // days reamaining to build end, 0 means end of build
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MsgBuildingsUpdates, colony_id, building_type, upgrade_of, days_remaining)
    MsgBuildingsUpdates();
    MsgBuildingsUpdates(int colony_id_, Building::BuildingType building_type_, Building::BuildingType upgrade_of_,
                        int days_remaining_);
};

struct MsgFleet
{
    unsigned int id;
    unsigned int player_id;  // owner
    Point position;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MsgFleet, id, player_id, position)
    MsgFleet();
    MsgFleet(const std::shared_ptr<Fleet>& fleet, unsigned int player_id_);
};

struct MsgFleetsJoin
{
    unsigned int joined_fleet_id_1;
    unsigned int joined_fleet_id_2;
    unsigned int result_fleet_id;
    Point result_fleet_pos;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MsgFleetsJoin, joined_fleet_id_1, joined_fleet_id_2, result_fleet_id,
                                   result_fleet_pos)
    MsgFleetsJoin();
    MsgFleetsJoin(const Sector::JoinedFleets& joined_fleets);
};

struct MsgWatchedSectorUpdate
{
    int sector_id;
    std::vector<MsgFleet> fleets;             // every fleet in sector, these which flew away, are just not shown
    std::vector<MsgFleetsJoin> joinedFleets;  // both player and other players joins
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MsgWatchedSectorUpdate, sector_id, fleets, joinedFleets)
    MsgWatchedSectorUpdate();
    MsgWatchedSectorUpdate(int sector_id_);
};

struct MsgBuildRequest
{
    int colony_id;
    Building::BuildingType building_type;
    Building::BuildingType upgrade_from;  // set if some building should be upgraded, otherwise, set to None
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MsgBuildRequest, colony_id, building_type, upgrade_from)
    MsgBuildRequest();
    MsgBuildRequest(int colony_id_, Building::BuildingType building_type_, Building::BuildingType upgrade_from_);
};

struct MsgMoveFleetRequest
{
    int fleet_id;
    Point position;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MsgMoveFleetRequest, fleet_id, position)
    MsgMoveFleetRequest();
    MsgMoveFleetRequest(int fleet_id_, Point position_);
};

struct MsgJoinFleetsRequest
{
    int fleet1_id;
    int fleet2_id;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MsgJoinFleetsRequest, fleet1_id, fleet2_id)
    MsgJoinFleetsRequest();
    MsgJoinFleetsRequest(int fleet1_id_, int fleet2_id_);
};

struct MsgFleetActionRequest
{
    int fleet_id;
    Fleet::Action action;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MsgFleetActionRequest, fleet_id, action)
    MsgFleetActionRequest();
    MsgFleetActionRequest(int fleet_id_, Fleet::Action action_);
};

}  // namespace messageTypes
