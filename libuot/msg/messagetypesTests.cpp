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

bool operator==(messageTypes::MsgTechnologyUpdate& t1, messageTypes::MsgTechnologyUpdate& t2)
{
    return t1.technology_type == t2.technology_type && t1.days_remaining == t2.days_remaining;
}

bool operator==(messageTypes::MsgBuildingsUpdates& b1, messageTypes::MsgBuildingsUpdates& b2)
{
    return b1.colony_id == b2.colony_id && b1.building_type == b2.building_type && b1.upgrade_of == b2.upgrade_of &&
           b1.days_remaining == b2.days_remaining;
}

bool operator==(messageTypes::MsgBuildRequest& b1, messageTypes::MsgBuildRequest& b2)
{
    return b1.colony_id == b2.colony_id && b1.building_type == b2.building_type && b1.upgrade_from == b2.upgrade_from;
}

bool operator==(messageTypes::MsgMoveFleetRequest& m1, messageTypes::MsgMoveFleetRequest& m2)
{
    return m1.fleet_id == m2.fleet_id && m1.position == m2.position;
}

bool operator==(messageTypes::MsgFleet& f1, messageTypes::MsgFleet& f2)
{
    return f1.id == f2.id && f1.player_id == f2.player_id && f1.position == f2.position;
}

bool operator==(messageTypes::MsgWatchedSectorUpdate& u1, messageTypes::MsgWatchedSectorUpdate& u2)
{
    if (u1.sector_id != u2.sector_id)
        return false;

    if (u1.fleets.size() != u2.fleets.size())
        return false;

    for (int i = 0; i < u1.fleets.size(); ++i)
        if (!(u1.fleets[i] == u2.fleets[i]))
            return false;

    return true;
}

void StartGamePayloadTest()
{
    messageTypes::StartGamePayload sgp;

    std::map<Resource, float> resources_t;
    std::set<PlanetaryFeatures::PlanetaryFeatureType> features = {
        PlanetaryFeatures::PlanetaryFeatureType::HotClimate,
        PlanetaryFeatures::PlanetaryFeatureType::MediumMetalsDeposits};
    auto planet =
        std::make_shared<Planet>(SectorObject(3, Point(5.0f, 6.0f), 3.0f), Planet::PlanetClimate::Hot, features);
    auto colony = std::make_shared<Colony>(33, planet);
    planet->colony = colony;

    auto player = std::make_shared<Player>(20, nullptr, resources_t, colony);
    colony->owner = player;

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

    sector1->objects.insert(planet);

    galaxy->sectors.insert(sector1);
    galaxy->sectors.insert(sector2);
    galaxy->sectors.insert(sector3);

    sgp.player_id = player->id;
    sgp.galaxy = messageTypes::MsgGalaxy(galaxy);

    sgp.starting_resources[Resource::Antimatter] = 11.0f;
    sgp.starting_resources[Resource::Crystals] = 21.0f;

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

    if (player->id != cast->player_id)
        std::cout << "StartGame - Wrong player id\n";

    auto antimatter = cast->starting_resources[Resource::Antimatter];
    auto crystals = cast->starting_resources[Resource::Crystals];
    if (antimatter != sgp.starting_resources[Resource::Antimatter] ||
        crystals != sgp.starting_resources[Resource::Crystals])
        std::cout << "StartGame - wrong resource values\n";
}

void NewTurnPayloadTest()
{
    messageTypes::NewTurnPayload ntp;

    float food_init = 100.0, metals_init = 10.0;
    float p1_init = 100.0, p10_init = 10.0;
    ntp.updated_resources[Resource::Food] = food_init;
    ntp.updated_resources[Resource::Metals] = metals_init;

    ntp.updated_populations[1] = p1_init;
    ntp.updated_populations[10] = p10_init;

    ntp.technology_update = {Technology::TechnologyType::HyperquantumPhysics, 10};

    messageTypes::MsgBuildingsUpdates buildUpdate1{2, Building::BuildingType::Greenhouses, Building::BuildingType::None,
                                                   3};
    messageTypes::MsgBuildingsUpdates buildUpdate2{3, Building::BuildingType::AdvancedMetalsMine,
                                                   Building::BuildingType::MetalsMine, 1};
    ntp.buildings_updates.push_back(buildUpdate1);
    ntp.buildings_updates.push_back(buildUpdate2);

    messageTypes::MsgWatchedSectorUpdate watchedSectorUpdate1{2};
    messageTypes::MsgWatchedSectorUpdate watchedSectorUpdate2{4};

    auto fl = std::make_shared<Fleet>();
    fl->id = 3;
    fl->position = {4.0f, 6.66f};
    messageTypes::MsgFleet fleet1{fl, 7};

    fl = std::make_shared<Fleet>();
    fl->id = 4;
    fl->position = {14.0f, 6.0f};
    messageTypes::MsgFleet fleet2{fl, 8};

    fl = std::make_shared<Fleet>();
    fl->id = 5;
    fl->position = {22.0f, 0.0f};
    messageTypes::MsgFleet fleet3{fl, 9};

    watchedSectorUpdate1.fleets.push_back(fleet1);
    watchedSectorUpdate1.fleets.push_back(fleet2);

    watchedSectorUpdate2.fleets.push_back(fleet3);

    ntp.watched_sectors_updates.push_back(watchedSectorUpdate1);
    ntp.watched_sectors_updates.push_back(watchedSectorUpdate2);

    auto ser = ntp.Serialize();
    std::shared_ptr<messageTypes::BasePayload> des = messageTypes::Deserialize(ser);
    auto type = des->GetType();
    if (type != messageTypes::MessageType::NewTurn)
        std::cout << "NewTurn - wrong message type\n";
    auto cast = std::dynamic_pointer_cast<messageTypes::NewTurnPayload>(des);

    if (cast->updated_resources.size() != 2)
        std::cout << "NewTurn - wrong resources size\n";

    if (cast->updated_populations.size() != 2)
        std::cout << "NewTurn - wrong populations size\n";

    if (cast->buildings_updates.size() != 2)
        std::cout << "NewTurn - wrong new buildings size\n";

    if (cast->watched_sectors_updates.size() != 2)
        std::cout << "NewTurn - wrong watched sectors size\n";

    auto food = cast->updated_resources[Resource::Food];
    auto metals = cast->updated_resources[Resource::Metals];
    if (food != food_init || metals != metals_init)
        std::cout << "NewTurn - wrong resource values\n";

    auto p1 = cast->updated_populations[1];
    auto p10 = cast->updated_populations[10];
    if (p1 != p1_init || p10 != p10_init)
        std::cout << "NewTurn - wrong people values\n";

    if (!(ntp.technology_update == cast->technology_update))
        std::cout << "NewTurn - wrong technology update\n";

    auto b1 = cast->buildings_updates[0];
    auto b2 = cast->buildings_updates[1];
    if (!(b1 == buildUpdate1) || !(b2 == buildUpdate2))
        std::cout << "NewTurn - wrong new buildings\n";

    auto ws1 = cast->watched_sectors_updates[0];
    auto ws2 = cast->watched_sectors_updates[1];
    if (!(ws1 == watchedSectorUpdate1) || !(ws2 == watchedSectorUpdate2))
        std::cout << "NewTurn - wrong watched sectors\n";
}

void ActionsPayloadTest()
{
    messageTypes::ActionsPayload ap;

    messageTypes::MsgBuildRequest buildRequest1 = {2, Building::BuildingType::Farms, Building::BuildingType::None};
    messageTypes::MsgBuildRequest buildRequest2 = {3, Building::BuildingType::AdvancedMetalsMine,
                                                   Building::BuildingType::MetalsMine};

    messageTypes::MsgMoveFleetRequest moveFleetRequest1 = {2, Point{44.0f, -41.0f}};

    ap.buildRequests.push_back(buildRequest1);
    ap.buildRequests.push_back(buildRequest2);

    ap.technologyRequest = Technology::TechnologyType::Engineering;

    ap.moveFleetRequests.push_back(moveFleetRequest1);

    auto ser = ap.Serialize();
    std::shared_ptr<messageTypes::BasePayload> des = messageTypes::Deserialize(ser);
    auto type = des->GetType();
    if (type != messageTypes::MessageType::Actions)
        std::cout << "Actions - wrong message type\n";
    auto cast = std::dynamic_pointer_cast<messageTypes::ActionsPayload>(des);

    if (!cast->createBaseActions.empty())
        std::cout << "Actions - wrong resources size\n";

    if (cast->technologyRequest != ap.technologyRequest)
        std::cout << "Actions - wrong technology request\n";
    if (cast->buildRequests.size() != 2)
        std::cout << "Actions - wrong build actions size\n";

    if (cast->moveFleetRequests.size() != 1)
        std::cout << "Actions - wrong move fleet actions size\n";

    auto b1 = cast->buildRequests[0];
    auto b2 = cast->buildRequests[1];
    if (!(b1 == buildRequest1) || !(b2 == buildRequest2))
        std::cout << "Actions - wrong build requests\n";

    if (!(cast->moveFleetRequests[0] == moveFleetRequest1))
        std::cout << "Actions - wrong move fleet requests\n";
}

void InvalidMessageTest()
{
    auto des = messageTypes::Deserialize("gswgnioewhfog");
    if (!!des)
        std::cout << "InvalidMessage - wrong behaviour\n";
}

int main()
{
    std::cout << "Tests started\n";

    NewTurnPayloadTest();
    ActionsPayloadTest();
    StartGamePayloadTest();
    InvalidMessageTest();

    std::cout << "Tests finished\n";
    std::cin.get();

    return 0;
}
