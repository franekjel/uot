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
    std::cout << "Message :: " << data << "\n";
#endif
    std::shared_ptr<messageTypes::BasePayload> des = messageTypes::Deserialize(data);
    auto type = des->GetType();
    switch (type)
    {
        case messageTypes::StartGame:
        {
            auto payload_start = std::dynamic_pointer_cast<messageTypes::StartGamePayload>(des);
            auto msgGalaxy = payload_start->galaxy;
            std::map<unsigned int, std::shared_ptr<Sector>> sectors_map;
            std::map<unsigned int, std::shared_ptr<Star>> stars_map;
            std::map<unsigned int, std::shared_ptr<Planet>> planets_map;
            std::map<unsigned int, std::shared_ptr<Colony>> colonies_map;
            std::map<unsigned int, std::shared_ptr<SpaceBase>> spaceBases_map;
            std::map<unsigned int, std::shared_ptr<InhabitableObject>> inhabitables_map;
            std::vector<std::shared_ptr<Sector>> sectors_vec;

            std::shared_ptr<Galaxy> galaxy_ptr = std::make_shared<Galaxy>();
            std::shared_ptr<Colony> colony_ptr = std::make_shared<Colony>(0, nullptr);
            unsigned int player_id = payload_start->player_id;
            std::shared_ptr<Player> player_ptr =
                std::make_shared<Player>(player_id, galaxy_ptr, std::map<Resource, float>{}, colony_ptr);

            for (int i = 0; i < msgGalaxy.sectors.size(); i++)
            {
                const auto s = std::make_shared<Sector>();
                s->sector_id = msgGalaxy.sectors[i].id;
                s->position = msgGalaxy.sectors[i].position;
                sectors_map.insert(std::pair<int, std::shared_ptr<Sector>>(s->sector_id, s));
                sectors_vec.push_back(s);
                context.gui->galaxy_stars_textures[s->sector_id] = UNDISCOVERED;
            }

            // Add "empty" sectors;
            for (int i = 0; i < msgGalaxy.sectors.size(); i++)
            {
                for (int j = 0; j < msgGalaxy.sectors[i].neighbors.size(); ++j)
                {
                    auto neigh = msgGalaxy.sectors[i].neighbors[j];

                    if (sectors_map.count(neigh.id) < 1)
                    {
                        const auto s = std::make_shared<Sector>();
                        s->sector_id = neigh.id;
                        s->position = neigh.position;
                        sectors_map.insert(std::pair<int, std::shared_ptr<Sector>>(s->sector_id, s));
                        sectors_vec.push_back(s);
                        context.gui->galaxy_stars_textures[neigh.id] = UNDISCOVERED;
                    }
                }
            }

            // Setting neighbors sectors references
            for (int i = 0; i < msgGalaxy.sectors.size(); i++)
            {
                auto& sec = sectors_map[msgGalaxy.sectors[i].id];
                for (int sec_idx = 0; sec_idx < msgGalaxy.sectors[i].neighbors.size(); sec_idx++)
                {
                    auto neighb_id = msgGalaxy.sectors[i].neighbors[sec_idx].id;
                    auto& neighb = sectors_map[neighb_id];
                    sec->neighbors.insert(neighb);
                    neighb->neighbors.insert(sec);
                }
            }

            // Setting stars for sectors
            for (int i = 0; i < msgGalaxy.sectors.size(); i++)
            {
                for (int j = 0; j < sectors_vec.size(); j++)
                {
                    if (msgGalaxy.sectors[i].id == sectors_vec[j]->sector_id)
                    {
                        std::vector<messageTypes::MsgStar> stars = msgGalaxy.sectors[i].stars;

                        for (int star_idx = 0; star_idx < stars.size(); star_idx++)
                        {
                            messageTypes::MsgStar star = stars[star_idx];
                            SectorObject _sectorObject =
                                SectorObject(star.id, star.position, star.object_size, msgGalaxy.sectors[i].id);
                            std::shared_ptr<Star> s_star = std::make_shared<Star>(_sectorObject, star.starType);
                            stars_map.insert(std::pair<int, std::shared_ptr<Star>>(star.id, s_star));
                            sectors_vec[j]->objects.insert({s_star->id, s_star});
                        }
                        if (!stars.empty())
                            context.gui->galaxy_stars_textures[msgGalaxy.sectors[i].id] =
                                context.gui->GetTextureIndex(sectors_vec[j]->objects.at(stars.front().id));
                    }
                }
            }

            // Setting planets for sectors
            for (int i = 0; i < msgGalaxy.sectors.size(); i++)
            {
                for (int j = 0; j < sectors_vec.size(); j++)
                {
                    if (msgGalaxy.sectors[i].id == sectors_vec[j]->sector_id)
                    {
                        std::vector<messageTypes::MsgPlanet> planets = msgGalaxy.sectors[i].planets;

                        for (int planet_idx = 0; planet_idx < planets.size(); planet_idx++)
                        {
                            messageTypes::MsgPlanet planet = planets[planet_idx];
                            SectorObject _sectorObject =
                                SectorObject(planet.id, planet.position, planet.object_size, msgGalaxy.sectors[i].id);

                            std::map<PlanetaryFeatures::PlanetaryFeatureType, int> planetary_features;
                            for (int feat = 0; feat < planet.planetary_features.size(); feat++)
                            {
                                std::map<PlanetaryFeatures::PlanetaryFeatureType, int>::iterator planetaryFeature =
                                    planet.planetary_features.begin();
                                std::advance(planetaryFeature, feat);
                                planetary_features.insert(*planetaryFeature);
                            }

                            auto _planet = std::make_shared<Planet>(_sectorObject, planet.climate, planetary_features);

                            // If there is colony
                            if (planet.colony_exists)
                            {
                                _planet->colony = std::make_shared<Colony>(0, nullptr);
                                _planet->colony->id = planet.colony.id;
                                _planet->colony->planet = _planet;
                                _planet->colony->owner = player_ptr;
                                _planet->colony->population = planet.colony.population;
                                _planet->colony->buildings = planet.colony.buildings;
                                _planet->planetary_features = planet.planetary_features;
                                player_ptr->owned_colonies[_planet->colony->id] = _planet->colony;
                                colonies_map.insert(
                                    std::pair<int, std::shared_ptr<Colony>>(_planet->colony->id, _planet->colony));
                            }

                            _planet->possible_buildings = planet.possible_buildings;
                            planets_map.insert(std::pair<int, std::shared_ptr<Planet>>(_planet->id, _planet));
                            sectors_vec[j]->objects.insert({_planet->id, _planet});
                        }
                    }
                }
            }

            // Setting inhabitables for sectors
            for (int i = 0; i < msgGalaxy.sectors.size(); i++)
            {
                for (int j = 0; j < sectors_vec.size(); j++)
                {
                    if (msgGalaxy.sectors[i].id == sectors_vec[j]->sector_id)
                    {
                        std::vector<messageTypes::MsgInhabitable> inhabitables = msgGalaxy.sectors[i].inhabitables;

                        for (int inhabitable_idx = 0; inhabitable_idx < inhabitables.size(); inhabitable_idx++)
                        {
                            messageTypes::MsgInhabitable inhabitable = inhabitables[inhabitable_idx];
                            SectorObject _sectorObject = SectorObject(inhabitable.id, inhabitable.position,
                                                                      inhabitable.object_size, msgGalaxy.sectors[i].id);
                            std::shared_ptr<InhabitableObject> s_inhabitable = std::make_shared<InhabitableObject>(
                                _sectorObject, inhabitable.resurce_deposit, inhabitable.object_type);

                            // If there is base
                            if (inhabitable.base_exists)
                            {
                                auto spaceBase = std::make_shared<SpaceBase>();
                                spaceBase->id = inhabitable.base.id;
                                // spaceBase.object TODO: CHECK THIS FIELD!!!
                                spaceBase->owner = player_ptr;
                                spaceBases_map.insert(
                                    std::pair<int, std::shared_ptr<SpaceBase>>(spaceBase->id, spaceBase));
                                s_inhabitable->base = spaceBase;
                            }

                            inhabitables_map.insert(
                                std::pair<int, std::shared_ptr<InhabitableObject>>(s_inhabitable->id, s_inhabitable));
                            sectors_vec[j]->objects.insert({s_inhabitable->id, s_inhabitable});
                        }
                    }
                }
            }

            for (int i = 0; i < sectors_vec.size(); i++)
            {
                galaxy_ptr->sectors.insert({sectors_vec[i]->sector_id, sectors_vec[i]});
            }

            for (const auto& u : payload_start->starting_ships_designs)
            {
                player_ptr->ship_designs.insert(
                    {u.id, std::make_shared<ShipDesign>(u.id, u.name, u.hull_type, u.sides, u.inside)});
            }

            auto state = context.getGameState();
            state.value->player = player_ptr;
            state.value->planets = planets_map;
        }
        break;

        case messageTypes::Actions:
        {
            auto payload_action = std::dynamic_pointer_cast<messageTypes::ActionsPayload>(des);
        }
        break;

        case messageTypes::NewTurn:
        {
            const auto payload_newturn = std::dynamic_pointer_cast<messageTypes::NewTurnPayload>(des);
            const auto& population_data = payload_newturn->updated_populations;
            const auto& resource_data = payload_newturn->updated_resources;
            const auto& sector_updates = payload_newturn->watched_sectors_updates;
            const auto& tech_data = payload_newturn->technology_updates;
            const auto& new_ships = payload_newturn->new_ships;
            const auto& fleet_jumps = payload_newturn->jumped_fleets;
            auto state = context.getGameState();
            if (state.value->player != nullptr)
            {
                for (const auto& pop_data : population_data)
                {
                    if (state.value->player->owned_colonies.find(pop_data.first) !=
                        state.value->player->owned_colonies.end())
                    {
                        state.value->player->owned_colonies.at(pop_data.first)->population = pop_data.second;
                    }
                }

                for (const auto& a : payload_newturn->buildings_updates)
                {
                    state.value->player->owned_colonies[a.colony_id]->building_queue.clear();
                }

                for (const auto& a : payload_newturn->buildings_updates)
                {
                    if (a.days_remaining == 0)
                    {
                        state.value->player->owned_colonies[a.colony_id]->buildings[a.building_type]++;
                    }
                    else
                    {
                        BuildingBuildProgress b{a.building_type, a.upgrade_of};
                        b.worker_week_units_left = a.days_remaining;

                        state.value->player->owned_colonies[a.colony_id]->building_queue.push_back(b);
                    }
                }

                for (const auto& r : resource_data)
                {
                    state.value->player->owned_resources[r.first] = r.second;
                }

                state.value->player->researched_technology.technology = Technology::TechnologyType::Empty;
                for (const auto& tech : tech_data)
                {
                    if (tech.days_remaining == 0 && tech.technology_type != Technology::TechnologyType::None &&
                        tech.technology_type != Technology::TechnologyType::Empty)
                    {
                        std::cout << "Finished tech " << static_cast<int>(tech.technology_type) << "\n";
                        state.value->player->DiscoverTechnology(tech.technology_type);
                        if (state.value->player->available_technologies.count(tech.technology_type) > 0)
                            state.value->player->available_technologies.erase(tech.technology_type);
                    }
                    state.value->player->researched_technology.technology = tech.technology_type;
                    state.value->player->researched_technology.progress_left = (float)tech.days_remaining;
                }

                for (const auto& s : new_ships)
                {
                    const auto& design = state.value->player->ship_designs.at(s.design_id);
                    std::shared_ptr<Ship> ship = Ship::ShipFromDesign(s.id, design);

                    std::shared_ptr<Fleet> fleet;
                    if (s.fleet_parameters.new_fleet)
                    {
                        auto sector = state.value->player->known_galaxy->sectors.at(
                            state.value->planets.at(s.planet_id)->sector_id);
                        const auto& p = s.fleet_parameters;
                        fleet = std::make_shared<Fleet>(p.id, sector, p.position, state.value->player->id);
                        state.value->player->owned_fleets.insert({fleet->id, fleet});
                        sector->present_fleets.insert({fleet->id, fleet});
                    }
                    else
                    {
                        fleet = state.value->player->owned_fleets.at(s.fleet_parameters.id);
                    }
                    fleet->ships.emplace_back(ship);

                    // TODO: this will be done be new fleetInfoRequest in future, leave now for tests
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

                for (const auto& u : sector_updates)
                {
                    auto s = state.value->player->known_galaxy->sectors[u.sector_id];
                    for (const auto& fu : u.fleets)
                    {
                        auto& f = s->present_fleets.at(fu.id);
                        f->position = fu.position;

                        const Point vec =
                            std::isnan(f->wanted_position.x) ? Point{0, 0} : f->wanted_position - f->position;
                        Point v;
                        if (vec.squaredLength() > (f->fleet_speed_per_turn * f->fleet_speed_per_turn))
                            v = vec.normalized() * f->fleet_speed_per_turn;
                        else
                            v = vec * f->fleet_speed_per_turn;
                        f->movement_vec = v;
                    }
                }

                for (const auto& fj : fleet_jumps)
                {
                    state.value->player->owned_fleets.find(fj.fleet_id)->second->location_sector
                        = state.value->player->known_galaxy->sectors.find(fj.sector_id_to)->second;
                }

                context.gui->last_turn_time = std::chrono::steady_clock::now();
                send_payload();
            }
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
