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
    unsigned int owner_id;
    unsigned int planet_id;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MsgColony, id, buildings, population, owner_id, planet_id)
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

struct MsgNeighbor
{
    int id;
    Point position;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MsgNeighbor, id, position)
    MsgNeighbor();
    MsgNeighbor(const std::shared_ptr<Sector>& sector);
};

struct MsgWarpZone
{
    unsigned int id;
    Point position;
    unsigned int sector_id;
    unsigned int connected_warp_zone_id;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MsgWarpZone, id, position, sector_id, connected_warp_zone_id)
    MsgWarpZone();
    MsgWarpZone(std::shared_ptr<WarpZone> warpZone);
};

struct MsgSector
{
    int id;
    Point position;
    std::vector<MsgNeighbor> neighbors;
    std::vector<MsgStar> stars;
    std::vector<MsgInhabitable> inhabitables;
    std::vector<MsgPlanet> planets;
    std::vector<MsgWarpZone> warp_zones;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MsgSector, id, position, neighbors, stars, inhabitables, planets, warp_zones)
    MsgSector();
    MsgSector(const std::shared_ptr<Sector>& sector);
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
    bool delete_design;  // możliwość usunięcia designu, w oczywisty sposób podając istniejące id można też nadpisywać
                         // designy
    std::string name;
    ShipHull::Type hull_type;
    std::map<ModuleType, int> sides;
    std::map<ModuleType, int> inside;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MsgShipDesign, id, delete_design, name, hull_type, sides, inside)
    MsgShipDesign();
    MsgShipDesign(const std::shared_ptr<ShipDesign>& design, bool delete_design_);
};

struct MsgCreateShip
{
    unsigned int design_id;
    unsigned int planet_id;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MsgCreateShip, design_id, planet_id)
    MsgCreateShip();
    MsgCreateShip(unsigned int design_id_, unsigned int planet_id_);
};

struct MsgShipDesignResponse
{
    unsigned int id;
    bool deleted;
    std::string name;
    ShipHull::Type hull_type;
    std::map<ModuleType, int> sides;
    std::map<ModuleType, int> inside;
    std::map<Resource, float> cost;
    std::map<Resource, float> upkeep;
    float worker_weeks_cost;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MsgShipDesignResponse, id, deleted, name, hull_type, sides, inside, cost, upkeep,
                                   worker_weeks_cost)
    MsgShipDesignResponse();
    MsgShipDesignResponse(unsigned int design_id_, const std::shared_ptr<ShipDesign>& design, bool deleted_);
};

struct MsgFleetParameters
{
    bool new_fleet;
    unsigned int id;
    Point position;
    float human_capacity;
    float construction_points;
    float base_fleet_speed;
    float current_hp;
    float max_hp;
    float current_shields;
    float max_shields;
    float average_energy;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MsgFleetParameters, new_fleet, id, position, human_capacity, construction_points,
                                   base_fleet_speed, current_hp, max_hp, current_shields, max_shields, average_energy)
    MsgFleetParameters();
    MsgFleetParameters(const Sector::FleetParameters& fleet_parameters, bool new_fleet_);
};

struct MsgChangedFleetPopulation
{
    unsigned int id;
    float soldiers;
    float civilians;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MsgChangedFleetPopulation, id, soldiers, civilians)
    MsgChangedFleetPopulation();
    MsgChangedFleetPopulation(const Sector::FleetPopChange& fleet);
};

struct MsgShipUpdate
{
    unsigned int design_id;
    unsigned int planet_id;
    int days_remaining;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MsgShipUpdate, design_id, planet_id, days_remaining);
    MsgShipUpdate();
    MsgShipUpdate(unsigned int design_id_, unsigned int planet_id_, int days_remaining);
};

struct MsgNewShip
{
    unsigned int id;
    unsigned int design_id;
    unsigned int planet_id;
    MsgFleetParameters fleet_parameters;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MsgNewShip, id, design_id, planet_id, fleet_parameters);
    MsgNewShip();
    MsgNewShip(unsigned int design_id_, unsigned int planet_id_, bool new_fleet, unsigned int ship_id_,
               const Sector::FleetParameters& fleet_parameters_);
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
    unsigned int colony_id;
    Building::BuildingType building_type;
    Building::BuildingType upgrade_of;  // set if new building is upgrade of another, otherwise, set to None
    int days_remaining;                 // days reamaining to build end, 0 means end of build
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MsgBuildingsUpdates, colony_id, building_type, upgrade_of, days_remaining)
    MsgBuildingsUpdates();
    MsgBuildingsUpdates(int colony_id_, Building::BuildingType building_type_, Building::BuildingType upgrade_of_,
                        int days_remaining_);
};

struct MsgWarpZoneUpdates
{
    unsigned int sector_id;
    Point position;
    int days_remaining;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MsgWarpZoneUpdates, sector_id, position, days_remaining)
    MsgWarpZoneUpdates();
    MsgWarpZoneUpdates(int sector_id_, Point position_, int days_remaining_);
};

struct MsgFleet
{
    unsigned int id;
    unsigned int player_id;  // owner
    Point position;
    Point predicted_position;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MsgFleet, id, player_id, position, predicted_position)
    MsgFleet();
    MsgFleet(const std::shared_ptr<Fleet>& fleet, unsigned int player_id_);
};

struct MsgFleetsJoin
{
    unsigned int joined_fleet_id_1;
    unsigned int joined_fleet_id_2;
    MsgFleetParameters fleet_parameters;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MsgFleetsJoin, joined_fleet_id_1, joined_fleet_id_2, fleet_parameters)
    MsgFleetsJoin();
    MsgFleetsJoin(const Sector::JoinedFleets& joined_fleets);
};

struct MsgFleetsJump
{
    unsigned int fleet_id;
    unsigned int sector_id_from;
    unsigned int sector_id_to;
    Point position;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MsgFleetsJump, fleet_id, sector_id_from, sector_id_to, position)
    MsgFleetsJump();
    MsgFleetsJump(const Sector::JumpedFleet& jumped_fleet);
};

struct MsgNewBase
{
    unsigned int base_id;
    unsigned int object_id;
    unsigned int owner;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MsgNewBase, base_id, object_id, owner)
    MsgNewBase();
    MsgNewBase(const Sector::NewBase& new_base);
};

struct MsgNewColony
{
    unsigned int colony_id;
    unsigned int object_id;
    unsigned int owner;
    float population;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MsgNewColony, colony_id, object_id, owner, population)
    MsgNewColony();
    MsgNewColony(const Sector::NewColony& new_colony);
};

struct MsgWatchedSectorUpdate
{
    unsigned int sector_id;
    std::vector<MsgFleet> fleets;  // every fleet in sector, these which flew away, are just not shown
    std::vector<MsgNewColony> new_colonies;
    std::vector<MsgNewBase> new_bases;
    std::vector<MsgFleetParameters> fleets_in_fight;
    std::vector<MsgWarpZone> warp_zones;  // new or just connected
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MsgWatchedSectorUpdate, sector_id, fleets, new_colonies, new_bases, fleets_in_fight,
                                   warp_zones)
    MsgWatchedSectorUpdate();
    MsgWatchedSectorUpdate(int sector_id_);
};

struct MsgBuildRequest
{
    unsigned int colony_id;
    Building::BuildingType building_type;
    Building::BuildingType upgrade_from;  // set if some building should be upgraded, otherwise, set to None
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MsgBuildRequest, colony_id, building_type, upgrade_from)
    MsgBuildRequest();
    MsgBuildRequest(int colony_id_, Building::BuildingType building_type_, Building::BuildingType upgrade_from_);
};

struct MsgMoveFleetRequest
{
    unsigned int fleet_id;
    Point position;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MsgMoveFleetRequest, fleet_id, position)
    MsgMoveFleetRequest();
    MsgMoveFleetRequest(int fleet_id_, Point position_);
};

struct MsgJoinFleetsRequest
{
    unsigned int fleet1_id;
    unsigned int fleet2_id;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MsgJoinFleetsRequest, fleet1_id, fleet2_id)
    MsgJoinFleetsRequest();
    MsgJoinFleetsRequest(int fleet1_id_, int fleet2_id_);
};

struct MsgFleetActionRequest
{
    unsigned int fleet_id;
    Fleet::Action action;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MsgFleetActionRequest, fleet_id, action)
    MsgFleetActionRequest();
    MsgFleetActionRequest(int fleet_id_, Fleet::Action action_);
};

struct MsgDetailedShipInfo
{
    unsigned int id;
    float hp;
    float max_hp;
    float hp_regen;
    float shield;
    float max_shield;
    float energy;
    float energy_regen;
    float max_energy;
    float soldiers;
    float civilians;
    float human_capacity;
    float speed;

    unsigned int design_id;
    std::map<ModuleType, int> weapons;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MsgDetailedShipInfo, id, hp, max_hp, hp_regen, shield, max_shield, energy,
                                   energy_regen, max_energy, soldiers, civilians, human_capacity, speed, design_id,
                                   weapons)
    MsgDetailedShipInfo();
    MsgDetailedShipInfo(std::shared_ptr<Ship> ship);
};

struct MsgDetailedFleetInfo
{
    unsigned int id;
    float soldiers;
    float civilians;
    float human_capacity;
    Point position;
    Fleet::Action current_action;
    std::vector<MsgDetailedShipInfo> ships;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MsgDetailedFleetInfo, id, soldiers, civilians, human_capacity, position,
                                   current_action, ships)
    MsgDetailedFleetInfo();
    MsgDetailedFleetInfo(std::shared_ptr<Fleet> fleet);
};

}  // namespace messageTypes
