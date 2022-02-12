#include "uot_net_client.h"
#include <array>
#include <iostream>

#include "game_state.h"
#include "msg/messagetypes.h"
#include "msg_queue.h"

void uot_net_client::connect_to_server() { run(); }

void uot_net_client::disconnect_from_server()
{
    disconnected = true;
    txrx.disconnect();
}

void uot_net_client::run()
{
    disconnected = false;
    txrx.connect("127.0.0.1", 7645);
}

uot_net_client::uot_net_client(client_context& cc)
    : txrx(*this, 40), disconnected(true), getting_input(false), context(cc)
{
    srand(time(nullptr));
    int id = rand() % 1000 + 1;
    name = "player" + std::to_string(id);
    txrx.set_as_handler();
}

std::string uot_net_client::get_name()
{
    std::cout << "Sending name: " << name << "\n";
    return name;
}

void uot_net_client::handle_status_change(net_status status)
{
    if (status == net_client::net_status::ok)
    {
        std::cout << "Connection accepted :)\n";
        disconnected = false;
        getting_input = true;
    }
    else
    {
        std::cout << "Disconnected :(\n";
        getting_input = false;
        disconnected = true;
    }
}

void uot_net_client::handle_message(const std::string& data)
{
#ifdef UOT_DEBUG
    std::cout << "Message :: " << data << std::endl;
#endif

    std::shared_ptr<messageTypes::BasePayload> des = messageTypes::Deserialize(data);
    auto type = des->GetType();
    auto state = context.getGameState().value;
    switch (type)
    {
        case messageTypes::StartGame:
        {
            const auto start_msg = std::dynamic_pointer_cast<messageTypes::StartGamePayload>(des);
            const unsigned int player_id = start_msg->player_id;

            state->player = std::make_shared<Player>(start_msg->player_id, std::make_shared<Galaxy>(),
                                                     start_msg->starting_resources, std::make_shared<Colony>());

            for (int id = 1; id < 16; id++)  // create other players - to have valid pointers and correct ids
                state->all_players.insert({id, std::make_shared<Player>(id)});
            state->all_players[player_id] = state->player;

            for (const auto& sec_msg : start_msg->galaxy.sectors)
                parseSector(sec_msg);

            parseDesigns(state, start_msg->starting_ships_designs);
        }
        break;

        case messageTypes::Actions:
        {
            auto payload_action = std::dynamic_pointer_cast<messageTypes::ActionsPayload>(des);
        }
        break;

        case messageTypes::NewTurn:
        {
            const auto newturn_msg = std::dynamic_pointer_cast<messageTypes::NewTurnPayload>(des);

            for (const auto& r : newturn_msg->updated_resources)
                state->player->owned_resources[r.first] = r.second;

            for (const auto& pop_data : newturn_msg->updated_populations)
                state->player->owned_colonies.at(pop_data.first)->population = pop_data.second;

            parseInvadedColonies(state, newturn_msg->invaded_colonies);

            parseBuildingsUpdates(state, newturn_msg->buildings_updates);

            parseTechnologyUpdate(state, newturn_msg->technology_updates);

            parseNewShipsUpdate(state, newturn_msg->new_ships);

            parseShipsUpdates(state, newturn_msg->ships_updates);

            parseFleetsInFight(state, newturn_msg->fleets_in_fight);

            parseFleetsPopulationChanges(state, newturn_msg->changed_fleet_populations);

            parseDestroyedShips(state, newturn_msg->destroyed_ships);

            parseJumpedFleets(state, newturn_msg->jumped_fleets);

            parseWatchedSectorUpdate(state, newturn_msg->watched_sectors_updates);

            parseJoinedFleets(state, newturn_msg->joined_fleets);

            for (const auto& i : newturn_msg->fleet_info_response)
                parseFleetInfo(state->player->owned_fleets.at(i.id), i);

            for (const auto& sec_msg : newturn_msg->new_sectors)
                parseSector(sec_msg);

            parseDesigns(state, newturn_msg->ship_designs);

            parseLostObjects(state, newturn_msg->lost_objects);

            ///
            ///

            context.gui->last_turn_time = std::chrono::steady_clock::now();
            context.gui->do_gui_per_turn_update(context);
            send_payload();
        }
        break;
        case messageTypes::None:
            break;
    }
}

void uot_net_client::send_payload()
{
    auto message_to_send = context.getActionQueue();
    auto msg_str = message_to_send.value->actions.Serialize();
    message_to_send.value->reset();

    txrx.send_reliable(msg_str);
}

void uot_net_client::updateFleet(const std::shared_ptr<Fleet> f, const messageTypes::MsgFleetParameters p)
{
    f->position = p.position;
    f->human_capacity = p.human_capacity;
    f->construction_points = p.construction_points;
    f->fleet_speed_per_turn = p.base_fleet_speed;
    f->current_hp = p.current_hp;
    f->max_hp = p.max_hp;
    f->current_shields = p.current_shields;
    f->max_shields = p.max_shields;
    f->average_energy = p.average_energy;
}

void uot_net_client::parseSector(const messageTypes::MsgSector& sec_msg)
{
    auto& state = context.getGameState().value;
    auto& player = state->player;
    auto& galaxy = player->known_galaxy->sectors;
    const unsigned int player_id = player->id;

    std::shared_ptr<Sector> s;

    if (galaxy.find(sec_msg.id) !=
        galaxy.end())  // if sector already exists (because eg. this is neighbour sector) only modify existing
    {
        s = galaxy.at(sec_msg.id);
    }
    else  // otherwise create and add new sector
    {
        s = std::make_shared<Sector>(sec_msg.id, sec_msg.position);
        galaxy[s->sector_id] = s;
    }

    if (s->neighbors.empty())
    {  // iterate over neighbours and create them if necessary
        for (const auto& n : sec_msg.neighbors)
        {
            auto p = galaxy.find(n.id);
            std::shared_ptr<Sector> neigh;
            if (p == galaxy.end())
            {
                neigh = std::make_shared<Sector>(n.id, n.position);
                context.gui->galaxy_stars_textures[n.id] = UNDISCOVERED;
                galaxy[n.id] = neigh;
            }
            else
            {
                neigh = p->second;
            }
            s->neighbors.insert(neigh);
        }
    }

    {  // star
        messageTypes::MsgStar s_msg = sec_msg.stars.front();
        std::shared_ptr<Star> star = std::make_shared<Star>(
            SectorObject(s_msg.id, s_msg.position, s_msg.object_size, sec_msg.id), s_msg.starType);
        s->objects.insert({star->id, star});
        context.gui->galaxy_stars_textures[s->sector_id] = context.gui->GetTextureIndex(star);
    }

    {  // planets
        std::vector<messageTypes::MsgPlanet> planets_msg = sec_msg.planets;

        for (const auto& p_msg : sec_msg.planets)
        {
            std::shared_ptr<Planet> planet =
                std::make_shared<Planet>(SectorObject(p_msg.id, p_msg.position, p_msg.object_size, sec_msg.id),
                                         p_msg.climate, p_msg.planetary_features);
            if (p_msg.colony_exists)
            {
                const auto& c_msg = p_msg.colony;
                std::shared_ptr<Colony> colony = std::make_shared<Colony>(c_msg.id, planet);
                colony->owner = state->all_players.at(c_msg.owner_id);
                colony->population = c_msg.population;
                colony->buildings = c_msg.buildings;
                if (c_msg.owner_id == player_id)
                    player->owned_colonies[c_msg.id] = colony;
                planet->colony = colony;
            }
            s->objects.insert({planet->id, planet});
            state->planets.insert({planet->id, planet});
        }
    }

    {  // inhabitable objects
        for (const auto& inh_msg : sec_msg.inhabitables)
        {
            std::shared_ptr<InhabitableObject> inhabitable = std::make_shared<InhabitableObject>(
                SectorObject(inh_msg.id, inh_msg.position, inh_msg.object_size, sec_msg.id), inh_msg.resurce_deposit,
                inh_msg.object_type);
            if (inh_msg.base_exists)
            {
                std::shared_ptr<SpaceBase> base = std::make_shared<SpaceBase>();
                base->id = inh_msg.base.id;
                base->owner = state->all_players.at(inh_msg.base.owner_id);
                inhabitable->base = base;
                if (base->owner->id == player->id)
                    player->owned_space_bases[base->id] = base;
            }
            s->objects.insert({inhabitable->id, inhabitable});
        }
    }
}

void uot_net_client::parseBuildingsUpdates(std::shared_ptr<game_state>& state,
                                           const std::vector<messageTypes::MsgBuildingsUpdates>& buildings_updates)
{
    for (const auto& a : buildings_updates)
    {
        state->player->owned_colonies[a.colony_id]->building_queue.clear();
    }

    for (const auto& a : buildings_updates)
    {
        if (a.days_remaining == 0)
        {
            state->player->owned_colonies[a.colony_id]->buildings[a.building_type]++;
        }
        else
        {
            BuildingBuildProgress b{a.building_type, a.upgrade_of};
            b.worker_week_units_left = a.days_remaining;

            state->player->owned_colonies[a.colony_id]->building_queue.push_back(b);
        }
    }
}

void uot_net_client::parseShipsUpdates(std::shared_ptr<game_state>& state,
                                       const std::vector<messageTypes::MsgShipUpdate>& ships_updates)
{
    // TODO: should it be like parseBuildingsUpdates? @Pawel?
}

void uot_net_client::parseTechnologyUpdate(std::shared_ptr<game_state>& state,
                                           const std::vector<messageTypes::MsgTechnologyUpdate>& technology_updates)
{
    state->player->researched_technology.technology = Technology::TechnologyType::Empty;
    for (const auto& tech : technology_updates)
    {
        if (tech.days_remaining == 0 && tech.technology_type != Technology::TechnologyType::None &&
            tech.technology_type != Technology::TechnologyType::Empty)
        {
            std::cout << "Finished tech " << static_cast<int>(tech.technology_type) << "\n";
            state->player->DiscoverTechnology(tech.technology_type);
            if (state->player->available_technologies.count(tech.technology_type) > 0)
                state->player->available_technologies.erase(tech.technology_type);
        }
        state->player->researched_technology.technology = tech.technology_type;
        state->player->researched_technology.progress_left = (float)tech.days_remaining;
    }
}

void uot_net_client::parseNewShipsUpdate(std::shared_ptr<game_state>& state,
                                         const std::vector<messageTypes::MsgNewShip>& new_ships)
{
    for (const auto& s : new_ships)
    {
        const auto& design = state->player->ship_designs.at(s.design_id);
        std::shared_ptr<Ship> ship = Ship::ShipFromDesign(s.id, design);

        std::shared_ptr<Fleet> fleet;
        if (s.fleet_parameters.new_fleet)
        {
            auto sector = state->player->known_galaxy->sectors.at(state->planets.at(s.planet_id)->sector_id);
            const auto& p = s.fleet_parameters;
            fleet = std::make_shared<Fleet>(p.id, sector, p.position, state->player->id);
            state->player->owned_fleets.insert({fleet->id, fleet});
            sector->present_fleets.insert({fleet->id, fleet});
        }
        else
        {
            fleet = state->player->owned_fleets.at(s.fleet_parameters.id);
        }
        ship->fleet = fleet;
        fleet->ships.insert({ship->id, ship});
        state->ships.insert({ship->id, ship});

        for (const auto& [t, c] : design->sides)
        {
            const auto& m = Modules.at(t);
            if (m.weapon.has_value())
            {
                fleet->fleet_weapons[t].first += c;
            }
        }

        updateFleet(fleet, s.fleet_parameters);
    }
}

void uot_net_client::parseWatchedSectorUpdate(
    std::shared_ptr<game_state>& state,
    const std::vector<messageTypes::MsgWatchedSectorUpdate>& watched_sectors_updates)
{
    const unsigned int player_id = state->player->id;
    std::map<unsigned int, std::shared_ptr<Fleet>> enemies_fleet_in_current_sector;
    for (const auto& u : watched_sectors_updates)
    {
        auto s = state->player->known_galaxy->sectors[u.sector_id];
        for (const auto& fu : u.fleets)
        {
            if (fu.player_id != player_id)
            {
                continue;
            }

            auto& f = s->present_fleets.at(fu.id);
            f->position = fu.position;

            const Point vec = std::isnan(f->wanted_position.x) ? Point{0, 0} : f->wanted_position - f->position;
            Point v;
            if (vec.squaredLength() > (f->fleet_speed_per_turn * f->fleet_speed_per_turn))
                v = vec.normalized() * f->fleet_speed_per_turn;
            else
                v = vec * f->fleet_speed_per_turn;
            f->movement_vec = v;
        }
        if (context.gui->current_sector.has_value() && context.gui->current_sector.value()->sector_id == u.sector_id)
        {
            for (const auto& fu : u.fleets)
            {
                if (fu.player_id != player_id)
                {
                    if (state->enemies_fleet_in_current_sector.count(fu.id))
                    {
                        enemies_fleet_in_current_sector.insert(state->enemies_fleet_in_current_sector.extract(fu.id));
                        enemies_fleet_in_current_sector[fu.id]->position = fu.position;
                    }
                    else
                        enemies_fleet_in_current_sector[fu.id] = std::make_shared<Fleet>(
                            fu.id, state->player->known_galaxy->sectors.at(u.sector_id), fu.position, fu.player_id);
                    enemies_fleet_in_current_sector.at(fu.id)->wanted_position = fu.predicted_position;
                }
            }
        }
    }
    state->enemies_fleet_in_current_sector = enemies_fleet_in_current_sector;
}

void uot_net_client::parseDestroyedShips(std::shared_ptr<game_state>& state,
                                         const std::vector<unsigned int>& destroyed_ships)
{
    for (const auto id : destroyed_ships)
    {
        auto& fleet = state->ships.at(id)->fleet;
        fleet->ships.erase(id);
        state->ships.erase(id);
        if (fleet->ships.empty())
        {
            if (context.gui->current_fleet.has_value() && fleet->id == context.gui->current_fleet.value()->id)
                context.gui->current_fleet.reset();

            state->player->owned_fleets.erase(fleet->id);
            fleet->location_sector->present_fleets.erase(fleet->id);
        }
    }
}

void uot_net_client::parseDesigns(std::shared_ptr<game_state>& state,
                                  const std::vector<messageTypes::MsgShipDesignResponse>& ship_designs)
{
    for (const auto& design_msg : ship_designs)
    {
        state->player->ship_designs.insert(
            {design_msg.id, std::make_shared<ShipDesign>(design_msg.id, design_msg.name, design_msg.hull_type,
                                                         design_msg.sides, design_msg.inside)});
    }
}

void uot_net_client::parseFleetsInFight(std::shared_ptr<game_state>& state,
                                        const std::vector<messageTypes::MsgFleetParameters>& fleets)
{
    // TODO: Does this return only our fleet or all fleets?
}

void uot_net_client::parseFleetInfo(std::shared_ptr<Fleet> f, const messageTypes::MsgDetailedFleetInfo& i)
{
    f->position = i.position;
    f->wanted_position = i.wanted_position;
    f->human_capacity = i.human_capacity;
    f->civilians = i.civilians;
    f->soldiers = i.soldiers;
    f->current_action = i.current_action;

    // We can assume that we don't need to remove or add ships becaouse we handle joining fleets, new ships construction
    // and fights losses
    for (const auto& s_msg : i.ships)
    {
        std::shared_ptr<Ship> s = f->ships.at(s_msg.id);
        s->hp = s_msg.hp;
        s->energy = s_msg.energy;
        s->shield = s_msg.shield;
        s->max_hp = s_msg.max_hp;
        s->max_energy = s_msg.energy;
        s->max_shield = s_msg.max_shield;
        s->soldiers = s_msg.soldiers;
        s->civilians = s_msg.civilians;
        s->human_capacity = s_msg.human_capacity;
        s->speed = s_msg.speed;
    }
}

void uot_net_client::parseFleetsPopulationChanges(std::shared_ptr<game_state>& state,
                                                  const std::vector<messageTypes::MsgChangedFleetPopulation>& update)
{
    for (const auto& u : update)
    {
        std::shared_ptr<Fleet> f = state->player->owned_fleets.at(u.id);
        f->civilians = u.civilians;
        f->soldiers = u.soldiers;
    }
}

void uot_net_client::parseJoinedFleets(std::shared_ptr<game_state>& state,
                                       const std::vector<messageTypes::MsgFleetsJoin>& joined_fleets)
{
    for (const auto& j : joined_fleets)
    {
        auto& f1 = state->player->owned_fleets.at(j.joined_fleet_id_1);
        auto& f2 = state->player->owned_fleets.at(j.joined_fleet_id_1);
        for (const auto& [id, s] : f2->ships)
            s->fleet = f1;
        f1->ships.merge(f2->ships);
        updateFleet(f1, j.fleet_parameters);
        if (context.gui->current_fleet.has_value() && context.gui->current_fleet.value()->id == f2->id)
            context.gui->current_fleet.reset();
        f2->location_sector->present_fleets.erase(f2->id);
        state->player->owned_fleets.erase(f2->id);
    }
}

void uot_net_client::parseJumpedFleets(std::shared_ptr<game_state>& state,
                                       const std::vector<messageTypes::MsgFleetsJump>& jumped_fleets)
{
    for (const auto& fj : jumped_fleets)
    {
        auto& s1 = state->player->known_galaxy->sectors.at(fj.sector_id_from);
        auto& s2 = state->player->known_galaxy->sectors.at(fj.sector_id_to);
        auto& f = state->player->owned_fleets.at(fj.fleet_id);

        s2->present_fleets.insert({fj.fleet_id, f});
        s1->present_fleets.erase(fj.fleet_id);
        f->location_sector = s2;
        f->position = fj.position;
        f->wanted_position = f->position;
        f->movement_vec = Point{0, 0};
    }
}

void uot_net_client::parseLostObjects(std::shared_ptr<game_state>& state, const std::vector<unsigned int>& lost)
{
    // TODO: These are ids of sector_object? Comments say about colony id...
}

void uot_net_client::parseInvadedColonies(std::shared_ptr<game_state>& state,
                                          const std::vector<messageTypes::MsgColony>& invaded)
{
    // TODO: This is just update of soldier count? If so why that big message, simple struct {id, soldiers_number}
    // should be sufficient
}
