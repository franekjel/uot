#include <iostream>
#include "messagetypes.h"

bool operator==(std::shared_ptr<Star>& s1, messageTypes::MsgStar& s2)
{
    return s1->id == s2.id && s1->position == s2.position && s1->size == s2.object_size && s1->star_type == s2.starType;
}

bool operator==(std::shared_ptr<SpaceBase> b1, messageTypes::MsgBase& b2)
{
    if (!!b1 && b2.id == -1 || !b1 && b2.id != -1)
        return false;
    return b1->id == b2.id && b1->owner->id == b2.owner_id;
}

bool operator==(std::shared_ptr<InhabitableObject> i1, messageTypes::MsgInhabitable& i2)
{
    bool b = i1->id == i2.id && i1->position == i2.position && i1->size == i2.object_size &&
             i1->object_type == i2.object_type;
    if (!b)
        return false;

    if (i1->base == i2.base)
        return false;

    if (i1->resurce_deposit.size() != i2.resurce_deposit.size())
        return false;
    for (auto resource : i1->resurce_deposit)
        if (resource.second != i2.resurce_deposit[resource.first])
            return false;

    return true;
}

bool operator==(std::shared_ptr<Colony> c1, messageTypes::MsgColony& c2)
{
    if (!!c1 && c2.id == -1 || !c1 && c2.id != -1)
        return false;

    bool b = c1->population == c2.population && c1->owner->id == c2.owner_id;
    if (!b)
        return false;

    if (c1->buildings.size() != c2.buildings.size())
        return false;

    for (auto building : c1->buildings)
        if (building.second != c2.buildings[building.first])
            return false;

    return true;
}

bool operator==(std::shared_ptr<Planet> p1, messageTypes::MsgPlanet& p2)
{
    bool b = p1->id == p2.id && p1->position == p2.position && p1->SectorObject::size == p2.object_size &&
             p1->Planet::size == p2.planet_size && p1->climate == p2.climate;

    if (!b)
        return false;

    if (!(p1->colony == p2.colony))
        return false;

    if (p1->planetary_features.size() != p2.planetary_features.size())
        return false;

    for (auto feature : p1->planetary_features)
        if (p2.planetary_features.find(feature) == p2.planetary_features.end())
            return false;

    if (p1->possible_buildings.size() != p2.possible_buildings.size())
        return false;

    for (auto building : p1->possible_buildings)
        if (building.second != p2.possible_buildings[building.first])
            return false;

    return true;
}

void StartGamePayloadTest()
{
    messageTypes::StartGamePayload sgp;

    auto galaxy = std::make_shared<Galaxy>();
    auto sector1 = std::make_shared<Sector>();
    sector1->sector_id = 1;
    sector1->position = Point(1.0f, 2.0f);

    auto sector2 = std::make_shared<Sector>();
    sector2->sector_id = 2;
    auto sector3 = std::make_shared<Sector>();
    sector3->sector_id = 3;

    sector1->neighbors.insert(sector2);
    sector2->neighbors.insert(sector1);
    sector1->neighbors.insert(sector3);
    sector3->neighbors.insert(sector1);

    /*TODO initialize*/
    auto star = std::make_shared<Star>(SectorObject(), Star::StarType::Sunny);
    sector1->objects.insert(star);

    // auto inhabitable = std::make_shared<InhabitableObject>();
    // sector1->objects.insert(inhabitable);

    // auto planet = std::make_shared<Planet>();
    // sector1->objects.insert(planet);

    galaxy->sectors.insert(sector1);
    galaxy->sectors.insert(sector2);
    galaxy->sectors.insert(sector3);

    sgp.galaxy = messageTypes::MsgGalaxy(galaxy);
    auto ser = sgp.Serialize();
    std::shared_ptr<messageTypes::BasePayload> des = messageTypes::Deserialize(ser);
    auto type = des->GetType();
    if (type != messageTypes::MessageType::StartGame)
        std::cout << "StartGame - wrong message type\n";

    auto cast = std::dynamic_pointer_cast<messageTypes::StartGamePayload>(des);

    if (cast->galaxy.sectors.size() != 3)
        std::cout << "StartGame - wrong sectors count\n";

    auto s0 = cast->galaxy.sectors[0];
    if (s0.id != sector1->sector_id)
        std::cout << "StartGame - wrong sector id";

    if (!(s0.position == sector1->position))
        std::cout << "StartGame - wrong sector position\n";

    if (s0.neighbors_ids.size() != 2)
        std::cout << "StartGame - wrong neighbors size\n";

    if (s0.neighbors_ids[0] != sector2->sector_id || s0.neighbors_ids[1] != sector3->sector_id)
        std::cout << "StartGame - wrong neighbors ids\n";

    if (s0.stars.size() != 1)
        std::cout << "StartGame - wrong stars size\n";

    if (!(star == s0.stars[0]))
        std::cout << "Star not correct";

    if (s0.planets.size() != 0)
        std::cout << "StartGame - wrong planets size\n";

    if (s0.inhabitables.size() != 0)
        std::cout << "StartGame - wrong inhabitables size\n";
}

void NewTourPayloadTest()
{
    messageTypes::NewTourPayload ntp;

    float food_init = 100.0, metals_init = 10.0;
    float p1_init = 100.0, p10_init = 10.0;
    ntp.updated_resources[Resource::Food] = food_init;
    ntp.updated_resources[Resource::Metals] = metals_init;

    ntp.updated_populations[1] = p1_init;
    ntp.updated_populations[10] = p10_init;

    auto ser = ntp.Serialize();
    std::shared_ptr<messageTypes::BasePayload> des = messageTypes::Deserialize(ser);
    auto type = des->GetType();
    if (type != messageTypes::MessageType::NewTour)
        std::cout << "NewTour - wrong message type\n";
    auto cast = std::dynamic_pointer_cast<messageTypes::NewTourPayload>(des);

    if (cast->updated_resources.size() != 2)
        std::cout << "NewTour - wrong resources size\n";

    if (cast->updated_populations.size() != 2)
        std::cout << "NewTour - wrong populations size\n";

    auto food = cast->updated_resources[Resource::Food];
    auto metals = cast->updated_resources[Resource::Metals];
    if (food != food_init || metals != metals_init)
        std::cout << "NewTour - wrong resource values\n";

    auto p1 = cast->updated_populations[1];
    auto p10 = cast->updated_populations[10];
    if (p1 != p1_init || p10 != p10_init)
        std::cout << "NewTour - wrong people values\n";
}

void ActionsPayloadTest()
{
    messageTypes::ActionsPayload ap;

    auto ser = ap.Serialize();
    std::shared_ptr<messageTypes::BasePayload> des = messageTypes::Deserialize(ser);
    auto type = des->GetType();
    if (type != messageTypes::MessageType::Actions)
        std::cout << "Actions - wrong message type\n";
    auto cast = std::dynamic_pointer_cast<messageTypes::ActionsPayload>(des);

    if (!cast->createBaseActions.empty())
        std::cout << "Actions - wrong resources size\n";
}

int main()
{
    std::cout << "Tests started\n";

    NewTourPayloadTest();
    ActionsPayloadTest();
    StartGamePayloadTest();

    std::cout << "Tests finished\n";
    std::cin.get();

    return 0;
}