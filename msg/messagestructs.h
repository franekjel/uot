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
    MsgBase base;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MsgInhabitable, id, position, object_size, resurce_deposit, object_type, base)
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
    std::set<PlanetaryFeatures::PlanetaryFeatureType> planetary_features;
    std::map<Building::BuildingType, int> possible_buildings;
    MsgColony colony;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MsgPlanet, id, position, object_size, climate, planet_size, planetary_features,
                                   possible_buildings, colony)
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
    MsgSector(const std::shared_ptr<Sector>& sector);
};

struct MsgGalaxy
{
    std::vector<MsgSector> sectors;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MsgGalaxy, sectors)
    MsgGalaxy();
    MsgGalaxy(const Galaxy& galaxy);
};
}  // namespace messageTypes