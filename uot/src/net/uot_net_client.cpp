#include "uot_net_client.h"
#include <array>
#include <iostream>
#include <mutex>

#include "game_state.h"
#include "msg/messagetypes.h"
#include "msg_queue.h"

void uot_net_client::disconnect_from_server()
{
    disconnected = true;
    txrx.disconnect();
}

void uot_net_client::connect_to_server(std::string ipv4)
{
    disconnected = false;
    txrx.connect(ipv4, 7645);
}

uot_net_client::uot_net_client(client_context& cc)
    : context(cc), txrx(*this, 40), getting_input(false), disconnected(true)
{
    srand(time(nullptr));
    int id = rand() % 1000 + 1;
    name = "player" + std::to_string(id);
    gen = std::mt19937(dev());
    txrx.set_as_handler();
}

bool uot_net_client::ipv4_correct(std::string& ipv4)
{
    unsigned char a, b, c, d;
    return sscanf(ipv4.c_str(), "%hhu.%hhu.%hhu.%hhu", &a, &b, &c, &d) == 4;
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

            context.player_id = state->player->id;

            for (const auto& sec_msg : start_msg->galaxy.sectors)
                parseSector(sec_msg);

            parseDesigns(state, start_msg->starting_ships_designs);

            for (const auto& d : start_msg->starting_ships_designs)
                context.gui->current_design_id = std::max(context.gui->current_design_id, d.id + 1);
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

            parseBuildingsUpdates(state, newturn_msg->buildings_updates);

            parseTechnologyUpdate(state, newturn_msg->technology_updates);

            parseNewShipsUpdate(state, newturn_msg->new_ships);

            parseShipsUpdates(state, newturn_msg->ships_updates);

            parseFleetsPopulationChanges(state, newturn_msg->changed_fleet_populations);

            parseJumpedFleets(state, newturn_msg->jumped_fleets);

            parseWatchedSectorUpdate(state, newturn_msg->watched_sectors_updates);

            parseDestroyedShips(state, newturn_msg->destroyed_ships);

            parseJoinedFleets(state, newturn_msg->joined_fleets);

            for (const auto& i : newturn_msg->fleet_info_response)
                parseFleetInfo(state->player->owned_fleets.at(i.id), i);

            for (const auto& sec_msg : newturn_msg->new_sectors)
                parseSector(sec_msg);

            parseDesigns(state, newturn_msg->ship_designs);

            parseLostObjects(state, newturn_msg->lost_objects);

            parseInvadedColonies(state, newturn_msg->invaded_colonies);

            for (const auto& pop_data : newturn_msg->updated_populations)
                state->player->owned_colonies.at(pop_data.first)->population = pop_data.second;

            for (const auto& soldiers_data : newturn_msg->updated_soldiers)
                state->player->owned_colonies.at(soldiers_data.first)->soldiers = soldiers_data.second;

            ///
            ///

            if (context.gui->current_fleet.has_value())
            {
                auto mq = context.getActionQueue().value;
                mq->actions.fleetInfoRequests.push_back(context.gui->current_fleet.value()->id);
            }

            context.getGameState().value->player->is_winner = newturn_msg->you_won;
            context.getGameState().value->player->is_loser = newturn_msg->you_lost;

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
        s->objects.clear();
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
            state->objects.insert({inhabitable->id, inhabitable});
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
            if (a.building_type == Building::Soldier)
                continue;
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
    for (const auto& a : ships_updates)
    {
        state->planets[a.planet_id]->colony->ship_building_queue.clear();
    }

    for (const auto& a : ships_updates)
    {
        // MAGIC NUMBER, DEPENDS ON
        // LAST SERVER SENT VALUE
        if (a.days_remaining != 1)
        {
            ShipBuildProgress b{state->player->ship_designs[a.design_id],
                                state->player->known_galaxy->sectors.at(state->planets.at(a.planet_id)->sector_id)};

            b.worker_week_units_left = a.days_remaining;
            state->planets[a.planet_id]->colony->ship_building_queue.push_back(b);
        }
    }
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
            std::lock_guard pr_fleet(state->present_fleet_mutex);
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
            context.fleet_mutex.lock();
            std::map<unsigned int, std::shared_ptr<Fleet>> enemies_fleet_in_current_sector;
            for (const auto& fu : u.fleets)
            {
                if (fu.player_id == player_id)
                    continue;
                std::shared_ptr<Fleet> enemy_fleet;
                auto p = state->enemies_fleet_in_current_sector.find(fu.id);
                if (p != state->enemies_fleet_in_current_sector.end())
                {
                    enemy_fleet = p->second;
                    enemies_fleet_in_current_sector.insert(*p);
                    enemy_fleet->position = fu.position;
                }
                else
                {
                    enemy_fleet = std::make_shared<Fleet>(fu.id, state->player->known_galaxy->sectors.at(u.sector_id),
                                                          fu.position, fu.player_id);
                    enemies_fleet_in_current_sector[fu.id] = enemy_fleet;
                }
                enemy_fleet->wanted_position = fu.predicted_position;
                enemy_fleet->movement_vec = fu.predicted_position - fu.position;
            }
            std::unique_lock e_fleet(state->enemy_fleet_mutex);
            state->enemies_fleet_in_current_sector = std::move(enemies_fleet_in_current_sector);
            e_fleet.unlock();
            updateAnimations(state, u.fleets_in_fight);
            context.fleet_mutex.unlock();
        }

        for (const auto& nb : u.new_bases)
        {
            auto& obj = state->objects.at(nb.object_id);
            if (!obj->base)
                obj->base = std::make_shared<SpaceBase>(nb.base_id, obj, state->player);
            if (nb.owner == player_id)
            {
                obj->base->owner = state->player;
                state->player->owned_space_bases.insert({nb.base_id, obj->base});
            }
            else
                obj->base->owner = state->all_players.at(nb.owner);
        }
        for (const auto& nc : u.new_colonies)
        {
            auto& planet = state->planets.at(nc.object_id);
            if (!planet->colony)
                planet->colony = std::make_shared<Colony>(nc.colony_id, planet);
            planet->colony->population = nc.population;
            if (nc.owner == player_id)
            {
                planet->colony->owner = state->player;
                state->player->owned_colonies.insert({nc.colony_id, planet->colony});
            }
            else
                planet->colony->owner = state->all_players.at(nc.owner);
        }
    }
}

void uot_net_client::parseDestroyedShips(std::shared_ptr<game_state>& state,
                                         const std::vector<unsigned int>& destroyed_ships)
{
    for (const auto id : destroyed_ships)
    {
        auto& ship = state->ships.at(id);
        auto& fleet = ship->fleet;

        for (const auto& [t, c] : ship->design->sides)
        {
            const auto& m = Modules.at(t);
            if (m.weapon.has_value())
            {
                fleet->fleet_weapons[t].first -= c;
            }
            if (!fleet->fleet_weapons[t].first)
                fleet->fleet_weapons.erase(t);
        }

        fleet->ships.erase(id);
        state->ships.erase(id);
        if (fleet->ships.empty())
        {
            if (context.gui->current_fleet.has_value() && fleet->id == context.gui->current_fleet.value()->id)
                context.gui->current_fleet.reset();

            state->player->owned_fleets.erase(fleet->id);
            std::lock_guard pr_fleet(state->present_fleet_mutex);
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

void uot_net_client::parseFleetInfo(std::shared_ptr<Fleet> f, const messageTypes::MsgDetailedFleetInfo& i)
{
    f->position = i.position;
    f->human_capacity = i.human_capacity;
    f->civilians = i.civilians;
    f->soldiers = i.soldiers;
    f->current_action = i.current_action;
    if (f->current_action != f->Fleet::Action::None)
        f->wanted_position = f->position;

    // We can assume that we don't need to remove or add ships becaouse we handle joining fleets, new ships construction
    // and fights losses
    for (const auto& s_msg : i.ships)
    {
        std::shared_ptr<Ship> s = f->ships.at(s_msg.id);
        s->hp = s_msg.hp;
        s->energy = s_msg.energy;
        s->shield = s_msg.shield;
        s->max_hp = s_msg.max_hp;
        s->max_energy = s_msg.max_energy;
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
        {
            std::lock_guard pr_fleet(state->present_fleet_mutex);
            f2->location_sector->present_fleets.erase(f2->id);
        }
        state->player->owned_fleets.erase(f2->id);

        f1->fleet_weapons.clear();
        for (const auto& [id, s] : f1->ships)
        {
            for (const auto& [t, c] : s->design->sides)
            {
                const auto& m = Modules.at(t);
                if (m.weapon.has_value())
                {
                    f1->fleet_weapons[t].first += c;
                }
            }
        }
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

        std::unique_lock pr_fleet(state->present_fleet_mutex);
        s2->present_fleets.insert({fj.fleet_id, f});
        s1->present_fleets.erase(fj.fleet_id);
        pr_fleet.unlock();
        f->location_sector = s2;
        f->position = fj.position;
        f->wanted_position = f->position;
        f->movement_vec = Point{0, 0};
    }
}

void uot_net_client::parseLostObjects(std::shared_ptr<game_state>& state, const std::vector<unsigned int>& lost_objects)
{
    for (const auto& lost : lost_objects)
    {
        auto p = state->planets.find(lost);
        if (p != state->planets.end())
        {
            const unsigned int colony_id = p->second->colony->id;
            state->player->owned_colonies.erase(colony_id);
            continue;
        }
        auto q = state->objects.find(lost);
        if (q != state->objects.end())
        {
            const unsigned int base_id = q->second->base->id;
            state->player->owned_space_bases.erase(base_id);
        }
    }
}

void uot_net_client::parseInvadedColonies(std::shared_ptr<game_state>& state,
                                          const std::vector<messageTypes::MsgColony>& invaded)
{
    for (const auto& c : invaded)
    {
        auto& colony = state->planets.at(c.planet_id)->colony;
        colony->buildings = c.buildings;
        colony->population = c.population;
        colony->owner = state->player;
        colony->soldiers = 0.0f;
        state->player->owned_colonies[c.id] = colony;
    }
}

void uot_net_client::updateAnimations(std::shared_ptr<game_state>& state,
                                      const std::vector<messageTypes::MsgFleetParameters>& fleets_in_fight)
{
    static auto last_time = SDL_GetTicks();
    auto& anims = context.gui->fight_animations;
    printf("\n\nTHERE IS %d ANIMATIONS!!!\n\n", anims.size());
    for (auto it = anims.begin(), it_next = it; it != anims.end(); it = it_next)
    {
        it_next++;
        if (it->second->ToDelete())
            anims.erase(it);
    }

    for (const auto& in_fight : fleets_in_fight)
    {
        if(SDL_GetTicks() - last_time > 600) {
            if (Mix_PlayChannel(-1, context.gr->shot.get(), 0) == -1)
            {
                throw std::runtime_error("couldnt plat the click sound");
            }
            last_time = SDL_GetTicks();
        }
        auto& fleet = state->player->owned_fleets.at(in_fight.id);
        updateFleet(fleet, in_fight);

        for (const auto& [w, c] : fleet->fleet_weapons)
        {
            for (const auto& [other_fleet_id, other_fleet] : state->enemies_fleet_in_current_sector)
            {
                const float range = Modules.at(w).weapon.value().range;
                if (other_fleet_id != fleet->id && other_fleet->owner_id != fleet->owner_id &&
                    (other_fleet->position - fleet->position).squaredLength() <= range * range)
                {
                    switch (std::discrete_distribution<>({0, 3, 3, 1, 1, 1, 1})(gen))
                    {
                        case 0:
                            break;
                        case 1:
                            anims[context.gui->last_id++] =
                                Animation::MissileAnimation(fleet->position, other_fleet->position, context.gr);
                            break;
                        case 2:
                            anims[context.gui->last_id++] =
                                Animation::MissileAnimation(other_fleet->position, fleet->position, context.gr);
                            break;
                        case 3:
                            anims[context.gui->last_id++] = Animation::Explosion1Animation(fleet->position, context.gr);
                            break;
                        case 4:
                            anims[context.gui->last_id++] =
                                Animation::Explosion1Animation(other_fleet->position, context.gr);
                            break;
                        case 5:
                            anims[context.gui->last_id++] = Animation::Explosion2Animation(fleet->position, context.gr);
                            break;
                        case 6:
                            anims[context.gui->last_id++] =
                                Animation::Explosion2Animation(other_fleet->position, context.gr);
                            break;
                    }
                }
            }
        }
    }
}
