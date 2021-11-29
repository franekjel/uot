#include "messagestructs.h"

messageTypes::MsgStar::MsgStar() {}
messageTypes::MsgStar::MsgStar(const std::shared_ptr<Star>& star)
    : id(star->id), position(star->position), object_size(star->size), starType(star->star_type)
{
}

messageTypes::MsgBase::MsgBase() { id = -1; }
messageTypes::MsgBase::MsgBase(const std::shared_ptr<SpaceBase>& spaceBase)
    : id(spaceBase->id), owner_id(spaceBase->owner->id)
{
}

messageTypes::MsgInhabitable::MsgInhabitable() { resurce_deposit = {}; }
messageTypes::MsgInhabitable::MsgInhabitable(const std::shared_ptr<InhabitableObject>& inhabitable)
    : id(inhabitable->id),
      position(inhabitable->position),
      object_size(inhabitable->size),
      object_type(inhabitable->object_type)
{
    for (auto resource : inhabitable->resurce_deposit)
        resurce_deposit[resource.first] = resource.second;
}

messageTypes::MsgColony::MsgColony()
{
    buildings = {};
    id = -1;
}
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
        colony = MsgColony(planet->colony);

    for (auto feature : planet->planetary_features)
        planetary_features.insert(feature);

    for (auto building : planet->possible_buildings)
        possible_buildings[building.first] = building.second;
}

messageTypes::MsgSector::MsgSector()
{
    neighbors_ids = {};
    stars = {};
    inhabitables = {};
    planets = {};
}
messageTypes::MsgSector::MsgSector(const std::shared_ptr<Sector>& sector)
    : id(sector->sector_id), position(sector->position)
{
    for (auto neighbor : sector->neighbors)
        neighbors_ids.push_back(neighbor->sector_id);
    for (auto sector_object : sector->objects)
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
messageTypes::MsgGalaxy::MsgGalaxy(const std::shared_ptr<Galaxy>& galaxy)
{
    for (auto sector : galaxy->sectors)
        sectors.push_back(MsgSector(sector));
}