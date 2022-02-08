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

    if (i1->resource_deposit.size() != i2.resurce_deposit.size())
        return false;
    for (auto resource : i1->resource_deposit)
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
        if (p2.planetary_features.find(feature.first) == p2.planetary_features.end())
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

bool operator==(messageTypes::MsgNewBase& f1, messageTypes::MsgNewBase& f2)
{
    return f1.base_id == f2.base_id && f1.object_id == f2.object_id && f1.owner == f2.owner;
}

bool operator==(messageTypes::MsgNewColony& f1, messageTypes::MsgNewColony& f2)
{
    return f1.colony_id == f2.colony_id && f1.object_id == f2.object_id && f1.owner == f2.owner &&
           f1.population == f2.population;
}

bool operator==(messageTypes::MsgFleetParameters& f1, messageTypes::MsgFleetParameters& f2)
{
    if (f1.id != f2.id || f1.new_fleet != f2.new_fleet || f1.position != f2.position || f1.soldiers != f2.soldiers ||
        f1.civilians != f2.civilians || f1.human_capacity != f2.human_capacity ||
        f1.construction_points != f2.construction_points || f1.base_fleet_speed != f2.base_fleet_speed ||
        f1.current_hp != f2.current_hp || f1.max_hp != f2.max_hp)
        return false;

    return true;
}

bool operator==(messageTypes::MsgFleetsJoin& f1, messageTypes::MsgFleetsJoin& f2)
{
    return f1.joined_fleet_id_1 == f2.joined_fleet_id_1 && f1.joined_fleet_id_2 == f2.joined_fleet_id_2 &&
           f1.fleet_parameters == f2.fleet_parameters;
}

bool operator==(messageTypes::MsgFleetsJump& f1, messageTypes::MsgFleetsJump& f2)
{
    return f1.fleet_id == f2.fleet_id && f1.sector_id_from == f2.sector_id_from && f1.sector_id_to == f2.sector_id_to &&
           f1.position == f2.position;
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

    if (u1.new_bases.size() != u2.new_bases.size())
        return false;

    for (int i = 0; i < u1.new_bases.size(); ++i)
        if (!(u1.new_bases[i] == u2.new_bases[i]))
            return false;

    if (u1.new_colonies.size() != u2.new_colonies.size())
        return false;

    for (int i = 0; i < u1.new_colonies.size(); ++i)
        if (!(u1.new_colonies[i] == u2.new_colonies[i]))
            return false;

    return true;
}

bool operator==(messageTypes::MsgShipUpdate& s1, messageTypes::MsgShipUpdate& s2)
{
    if (s1.design_id != s2.design_id || s1.planet_id != s2.planet_id || s1.days_remaining != s2.days_remaining)
        return false;

    return true;
}

bool operator==(messageTypes::MsgNewShip& s1, messageTypes::MsgNewShip& s2)
{
    if (s1.id != s2.id || s1.design_id != s2.design_id || s1.planet_id != s2.planet_id)
        return false;

    if (!(s1.fleet_parameters == s2.fleet_parameters))
        return false;

    return true;
}

bool operator==(messageTypes::MsgShipDesignResponse& r1, messageTypes::MsgShipDesignResponse& r2)
{
    if (r1.id != r2.id || r1.deleted != r2.deleted || r1.name != r2.name || r1.hull_type != r2.hull_type ||
        r1.worker_weeks_cost != r2.worker_weeks_cost)
        return false;

    if (r1.sides.size() != r2.sides.size())
        return false;
    if (r1.inside.size() != r2.inside.size())
        return false;
    if (r1.cost.size() != r2.cost.size())
        return false;
    if (r1.upkeep.size() != r2.upkeep.size())
        return false;

    for (auto& [module_, count] : r1.sides)
    {
        if (r2.sides[module_] != count)
            return false;
    }

    for (auto& [module_, count] : r1.inside)
    {
        if (r2.inside[module_] != count)
            return false;
    }

    for (auto& [res, count] : r1.cost)
    {
        if (r2.cost[res] != count)
            return false;
    }

    for (auto& [res, count] : r1.upkeep)
    {
        if (r2.upkeep[res] != count)
            return false;
    }
    return true;
}
void StartGamePayloadTest()
{
    messageTypes::StartGamePayload sgp;

    std::map<Resource, float> resources_t;
    std::map<PlanetaryFeatures::PlanetaryFeatureType, int> features = {
        {PlanetaryFeatures::PlanetaryFeatureType::HotClimate, 1},
        {PlanetaryFeatures::PlanetaryFeatureType::MetalsDeposit, 1}};
    auto sector1 = std::make_shared<Sector>();
    sector1->sector_id = 1;
    auto planet = std::make_shared<Planet>(SectorObject(3, Point(5.0f, 6.0f), 3.0f, sector1->sector_id),
                                           Planet::PlanetClimate::Hot, features);
    auto colony = std::make_shared<Colony>(33, planet);
    planet->colony = colony;

    auto player = std::make_shared<Player>(20, nullptr, resources_t, colony);
    colony->owner = player;

    auto galaxy = std::make_shared<Galaxy>();
    sector1->position = Point(1.0f, 2.0f);

    auto sector2 = std::make_shared<Sector>();
    sector2->sector_id = 2;
    auto sector3 = std::make_shared<Sector>();
    sector3->sector_id = 3;

    sector1->neighbors.insert(sector2);
    sector2->neighbors.insert(sector1);
    sector1->neighbors.insert(sector3);
    sector3->neighbors.insert(sector1);

    auto star =
        std::make_shared<Star>(SectorObject(1, Point(1.0f, 1.0f), 1.0f, sector1->sector_id), Star::StarType::Sunny);
    sector1->objects.insert({star->id, star});

    std::map<Resource, float> resources = {{Resource::AncientNanobots, 1.0f}, {Resource::DarkMatter, 10.0f}};
    auto inhabitable =
        std::make_shared<InhabitableObject>(SectorObject(2, Point(-1.0f, -1.0f), 2.0f, sector1->sector_id), resources,
                                            InhabitableObject::ObjectType::GasGiant);
    auto base = std::make_shared<SpaceBase>(10, inhabitable, player);
    inhabitable->base = base;
    sector1->objects.insert({inhabitable->id, inhabitable});

    sector1->objects.insert({planet->id, planet});

    galaxy->sectors.insert({sector1->sector_id, sector1});
    galaxy->sectors.insert({sector2->sector_id, sector2});
    galaxy->sectors.insert({sector3->sector_id, sector3});

    sgp.player_id = player->id;
    sgp.galaxy = messageTypes::MsgGalaxy(galaxy, player);

    sgp.starting_resources[Resource::Antimatter] = 11.0f;
    sgp.starting_resources[Resource::Crystals] = 21.0f;

    messageTypes::MsgShipDesignResponse msdr;
    msdr.id = 1;
    msdr.deleted = false;
    msdr.name = "lul";
    msdr.hull_type = ShipHull::Type::MediumShipHull;
    msdr.sides[ModuleType::AdvancedBigLaser] = 3.0f;
    msdr.inside[ModuleType::AdvancedBigLaser] = 3.0f;
    msdr.cost[Resource::Antimatter] = 4.0f;
    msdr.upkeep[Resource::AncientRelics] = 5.0f;
    msdr.worker_weeks_cost = 6.4f;
    sgp.starting_ships_designs.push_back(msdr);

    auto ser = sgp.Serialize();
    std::shared_ptr<messageTypes::BasePayload> des = messageTypes::Deserialize(ser);
    auto type = des->GetType();
    if (type != messageTypes::MessageType::StartGame)
        std::cout << "StartGame - wrong message type\n";

    auto cast = std::dynamic_pointer_cast<messageTypes::StartGamePayload>(des);

    if (cast->galaxy.sectors.size() != 1)
        std::cout << "StartGame - wrong sectors count\n";

    auto s0 = cast->galaxy.sectors[0];
    if (s0.id != sector1->sector_id)
        std::cout << "StartGame - wrong sector id\n";

    if (!(s0.position == sector1->position))
        std::cout << "StartGame - wrong sector position\n";

    if (s0.neighbors.size() != 2)
        std::cout << "StartGame - wrong neighbors size\n";

    if ((s0.neighbors[0].id != sector2->sector_id || s0.neighbors[1].id != sector3->sector_id) &&
        (s0.neighbors[1].id != sector2->sector_id || s0.neighbors[0].id != sector3->sector_id))
        std::cout << "StartGame - wrong neighbors ids\n";

    if ((s0.neighbors[0].position != sector2->position || s0.neighbors[1].position != sector3->position) &&
        (s0.neighbors[1].position != sector2->position || s0.neighbors[0].position != sector3->position))
        std::cout << "StartGame - wrong neighbors pos\n";

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

    if (cast->starting_ships_designs.size() != 1)
        std::cout << "StartGame - wrong designs size\n";

    if (!(msdr == cast->starting_ships_designs[0]))
        std::cout << "StartGame - wrong design\n";

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

    messageTypes::MsgTechnologyUpdate technologyUpdate1 = {Technology::TechnologyType::AdvancedSpaceships, 10};
    messageTypes::MsgTechnologyUpdate technologyUpdate2 = {Technology::TechnologyType::AdvancedGenetics, 0};
    ntp.technology_updates = {technologyUpdate1, technologyUpdate2};

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

    Sector::NewBase nb{1, 2, 3};
    Sector::NewColony nc{4, 5, 6, 7.0f};

    watchedSectorUpdate1.new_bases.push_back(nb);
    watchedSectorUpdate1.new_colonies.push_back(nc);

    Sector::FleetParameters flp;
    flp.fleet_id = 1;
    flp.position = {0.0f, 2.0f};
    flp.soldiers = 4.0f;
    flp.civilians = 5.0f;
    flp.human_capacity = 6.0f;
    flp.construction_points = 8.0f;
    flp.base_fleet_speed = -1.0f;
    flp.current_hp = 11.0f;
    flp.max_hp = 33.0f;

    messageTypes::MsgFleetsJoin jf{{1, 2, 1, flp}};
    ntp.joined_fleets.push_back(jf);

    messageTypes::MsgFleetsJump juf{{1, 2, 1, {3.0, 3.0}}};
    ntp.jumped_fleets.push_back(juf);

    watchedSectorUpdate2.fleets.push_back(fleet3);

    ntp.watched_sectors_updates.push_back(watchedSectorUpdate1);
    ntp.watched_sectors_updates.push_back(watchedSectorUpdate2);

    messageTypes::MsgShipDesignResponse msdr;
    msdr.id = 1;
    msdr.deleted = false;
    msdr.name = "lul";
    msdr.hull_type = ShipHull::Type::MediumShipHull;
    msdr.sides[ModuleType::AdvancedBigLaser] = 3.0f;
    msdr.inside[ModuleType::AdvancedBigLaser] = 3.0f;
    msdr.cost[Resource::Antimatter] = 4.0f;
    msdr.upkeep[Resource::AncientRelics] = 5.0f;
    msdr.worker_weeks_cost = 6.4f;
    ntp.ship_designs.push_back(msdr);

    messageTypes::MsgShipUpdate msu;
    messageTypes::MsgNewShip mns;
    Sector::FleetParameters flp2;
    flp2.fleet_id = 3;
    flp2.position = {0.2f, 2.0f};
    flp2.soldiers = 4.1f;
    flp2.civilians = 8.0f;
    flp2.human_capacity = 3.9f;
    flp2.construction_points = 4.0f;
    flp2.base_fleet_speed = 4.0f;
    flp2.current_hp = 1.0f;
    flp2.max_hp = 24.0f;

    msu.design_id = 2;
    msu.planet_id = 3;
    msu.days_remaining = 0;
    ntp.ships_updates.push_back(msu);

    mns.id = 1;
    mns.design_id = 2;
    mns.planet_id = 3;
    mns.fleet_parameters = messageTypes::MsgFleetParameters(flp2, false);
    ntp.new_ships.push_back(mns);

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

    if (cast->joined_fleets.size() != 1)
        std::cout << "NewTurn - wrong joined fleets size\n";

    if (cast->jumped_fleets.size() != 1)
        std::cout << "NewTurn - wrong jumped fleets size\n";

    auto food = cast->updated_resources[Resource::Food];
    auto metals = cast->updated_resources[Resource::Metals];
    if (food != food_init || metals != metals_init)
        std::cout << "NewTurn - wrong resource values\n";

    auto p1 = cast->updated_populations[1];
    auto p10 = cast->updated_populations[10];
    if (p1 != p1_init || p10 != p10_init)
        std::cout << "NewTurn - wrong people values\n";

    auto t1 = cast->technology_updates[0];
    auto t2 = cast->technology_updates[1];
    if (!(t1 == technologyUpdate1) || !(t2 == technologyUpdate2))
        std::cout << "NewTurn - wrong technology update\n";

    auto b1 = cast->buildings_updates[0];
    auto b2 = cast->buildings_updates[1];
    if (!(b1 == buildUpdate1) || !(b2 == buildUpdate2))
        std::cout << "NewTurn - wrong new buildings\n";

    auto ws1 = cast->watched_sectors_updates[0];
    auto ws2 = cast->watched_sectors_updates[1];
    if (!(ws1 == watchedSectorUpdate1) || !(ws2 == watchedSectorUpdate2))
        std::cout << "NewTurn - wrong watched sectors\n";

    auto jf1 = cast->joined_fleets[0];
    if (!(jf1 == jf))
        std::cout << "NewTurn - wrong joined fleets\n";

    auto juf1 = cast->jumped_fleets[0];
    if (!(juf1 == juf))
        std::cout << "NewTurn - wrong joined fleets\n";

    if (cast->ship_designs.size() != 1)
        std::cout << "NewTurn - wrong designs size\n";

    if (!(msdr == cast->ship_designs[0]))
        std::cout << "NewTurn - wrong design\n";

    if (cast->ships_updates.size() != 1)
        std::cout << "NewTurn - wrong ships size\n";

    if (!(msu == cast->ships_updates[0]))
        std::cout << "NewTurn - wrong ship\n";

    if (cast->new_ships.size() != 1)
        std::cout << "NewTurn - wrong new ships size\n";

    if (!(mns == cast->new_ships[0]))
        std::cout << "NewTurn - wrong mew ship\n";
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

    // TODO join, fleetAction, shipDesign, createShip
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
