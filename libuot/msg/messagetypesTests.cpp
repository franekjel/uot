#include <iostream>
#include "messagetypes.h"

bool operator==(std::shared_ptr<Star>& s1, messageTypes::MsgStar& s2)
{
    return s1->id == s2.id && s1->position == s2.position && s1->size == s2.object_size && s1->star_type == s2.starType;
}

bool operator==(std::shared_ptr<SpaceBase> b1, messageTypes::MsgBase& b2)
{
    return b1->id == b2.id && b1->owner->id == b2.owner_id;
}

bool operator==(std::shared_ptr<InhabitableObject> i1, messageTypes::MsgInhabitable& i2)
{
    bool b = i1->id == i2.id && i1->position == i2.position && i1->size == i2.object_size &&
             i1->object_type == i2.object_type;
    if (!b)
        return false;

    if (!!i1->base && !i2.base_exists || !i1->base && i2.base_exists)
        return false;

    if (i2.base_exists)
        if (!(i1->base == i2.base))
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

    if (!!p1->colony && !p2.colony_exists || !p1->colony && p2.colony_exists)
        return false;

    if (p2.colony_exists)
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

bool operator==(messageTypes::MsgNewBuilding& b1, messageTypes::MsgNewBuilding& b2)
{
    return b1.colony_id == b2.colony_id && b1.building_type == b2.building_type && b1.upgrade_of == b2.upgrade_of &&
           b1.count == b2.count;
}

bool operator==(messageTypes::MsgBuildRequest& b1, messageTypes::MsgBuildRequest& b2)
{
    return b1.colony_id == b2.colony_id && b1.building_type == b2.building_type && b1.upgrade_from == b2.upgrade_from;
}

void StartGamePayloadTest()
{
    messageTypes::StartGamePayload sgp;

    std::map<Resource, float> resources_t;
    auto player = std::make_shared<Player>(20, nullptr, resources_t, nullptr);

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

    auto star = std::make_shared<Star>(SectorObject(1, Point(1.0f, 1.0f), 1.0f), Star::StarType::Sunny);
    sector1->objects.insert(star);

    std::map<Resource, float> resources = {{Resource::AncientNanobots, 1.0f}, {Resource::DarkMatter, 10.0f}};
    auto inhabitable = std::make_shared<InhabitableObject>(SectorObject(2, Point(-1.0f, -1.0f), 2.0f), resources,
                                                           InhabitableObject::ObjectType::GasGiant);
    auto base = std::make_shared<SpaceBase>();
    base->id = 10;
    base->owner = player;
    inhabitable->base = base;
    sector1->objects.insert(inhabitable);

    std::set<PlanetaryFeatures::PlanetaryFeatureType> features = {
        PlanetaryFeatures::PlanetaryFeatureType::HotClimate,
        PlanetaryFeatures::PlanetaryFeatureType::MediumMetalsDeposits};
    auto planet =
        std::make_shared<Planet>(SectorObject(3, Point(5.0f, 6.0f), 3.0f), Planet::PlanetClimate::Hot, features);
    auto colony = std::make_shared<Colony>(33, planet);
    colony->owner = player;
    planet->colony = colony;

    sector1->objects.insert(planet);

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
        std::cout << "StartGame - wrong sector id\n";

    if (!(s0.position == sector1->position))
        std::cout << "StartGame - wrong sector position\n";

    if (s0.neighbors_ids.size() != 2)
        std::cout << "StartGame - wrong neighbors size\n";

    if (s0.neighbors_ids[0] != sector2->sector_id || s0.neighbors_ids[1] != sector3->sector_id)
        std::cout << "StartGame - wrong neighbors ids\n";

    if (s0.stars.size() != 1)
        std::cout << "StartGame - wrong stars size\n";

    if (!(star == s0.stars[0]))
        std::cout << "StartGame - Star not correct\n";

    if (s0.inhabitables.size() != 1)
        std::cout << "StartGame - wrong inhabitables size\n";

    if (!(inhabitable == s0.inhabitables[0]))
        std::cout << "StartGame - Inhabitable not correct\n";

    if (s0.planets.size() != 1)
        std::cout << "StartGame - wrong planets size\n";

    if (!(planet == s0.planets[0]))
        std::cout << "StartGame - Planet not correct\n";
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

    messageTypes::MsgNewBuilding newBuilding1{2, Building::BuildingType::Greenhouses, Building::BuildingType::None, 3};
    messageTypes::MsgNewBuilding newBuilding2{3, Building::BuildingType::ImprovedMetalsMine,
                                              Building::BuildingType::MetalsMine, 1};
    ntp.new_buildings.push_back(newBuilding1);
    ntp.new_buildings.push_back(newBuilding2);

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

    if (cast->new_buildings.size() != 2)
        std::cout << "NewTour - wrong new buildings size\n";

    auto food = cast->updated_resources[Resource::Food];
    auto metals = cast->updated_resources[Resource::Metals];
    if (food != food_init || metals != metals_init)
        std::cout << "NewTour - wrong resource values\n";

    auto p1 = cast->updated_populations[1];
    auto p10 = cast->updated_populations[10];
    if (p1 != p1_init || p10 != p10_init)
        std::cout << "NewTour - wrong people values\n";

    auto b1 = cast->new_buildings[0];
    auto b2 = cast->new_buildings[1];
    if (!(b1 == newBuilding1) || !(b2 == newBuilding2))
        std::cout << "NewTour - wrong new buildings\n";
}

void ActionsPayloadTest()
{
    messageTypes::ActionsPayload ap;

    messageTypes::MsgBuildRequest buildRequest1 = {2, Building::BuildingType::Farm, Building::BuildingType::None};
    messageTypes::MsgBuildRequest buildRequest2 = {3, Building::BuildingType::ImprovedMetalsMine,
                                                   Building::BuildingType::MetalsMine};
    ap.buildRequests.push_back(buildRequest1);
    ap.buildRequests.push_back(buildRequest2);

    auto ser = ap.Serialize();
    std::shared_ptr<messageTypes::BasePayload> des = messageTypes::Deserialize(ser);
    auto type = des->GetType();
    if (type != messageTypes::MessageType::Actions)
        std::cout << "Actions - wrong message type\n";
    auto cast = std::dynamic_pointer_cast<messageTypes::ActionsPayload>(des);

    if (cast->buildRequests.size() != 2)
        std::cout << "Actions - wrong build actions size\n";

    auto b1 = cast->buildRequests[0];
    auto b2 = cast->buildRequests[1];
    if (!(b1 == buildRequest1) || !(b2 == buildRequest2))
        std::cout << "Actions - wrong build requests\n";
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