#include "uot_net_client.h"
#include <array>
#include <iostream>

#include "../libuot/msg/messagetypes.h"
#include "game_state.h"

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
    std::cout << "Message :: " << data << "\n";
    std::shared_ptr<messageTypes::BasePayload> des = messageTypes::Deserialize(data);
    auto type = des->GetType();
    switch (type)
    {
        case messageTypes::StartGame:
        {
            auto payload_start = std::dynamic_pointer_cast<messageTypes::StartGamePayload>(des);
            auto msgGalaxy = payload_start->galaxy;
            std::map<int, std::shared_ptr<Sector>> sectors_map;
            std::map<int, std::shared_ptr<Star>> stars_map;
            std::map<int, std::shared_ptr<Planet>> planets_map;
            std::map<int, std::shared_ptr<Colony>> colonies_map;
            std::map<int, std::shared_ptr<SpaceBase>> spaceBases_map;
            std::map<int, std::shared_ptr<InhabitableObject>> inhabitables_map;
            std::vector<std::shared_ptr<Sector>> sectors_vec;

            std::shared_ptr<Galaxy> galaxy_ptr = std::make_shared<Galaxy>();
            std::shared_ptr<Colony> colony_ptr = std::make_shared<Colony>(0, nullptr);
            long player_id = payload_start->player_id;
            std::map<Resource, float> resource_data;
            resource_data.insert(std::pair<Resource, float>(Resource::Metals, 0));
            resource_data.insert(std::pair<Resource, float>(Resource::Antimatter, 0));
            resource_data.insert(std::pair<Resource, float>(Resource::RareMetals, 0));
            resource_data.insert(std::pair<Resource, float>(Resource::Crystals, 0));
            resource_data.insert(std::pair<Resource, float>(Resource::Polymers, 0));
            resource_data.insert(std::pair<Resource, float>(Resource::DarkMatter, 0));
            resource_data.insert(std::pair<Resource, float>(Resource::AncientNanobots, 0));
            resource_data.insert(std::pair<Resource, float>(Resource::AncientRelics, 0));
            resource_data.insert(std::pair<Resource, float>(Resource::Spatium, 0));
            resource_data.insert(std::pair<Resource, float>(Resource::Food, 0));
            resource_data.insert(std::pair<Resource, float>(Resource::Technology, 0));

            std::shared_ptr<Player> player_ptr =
                std::make_shared<Player>(player_id, galaxy_ptr, resource_data, colony_ptr);

            for (int i = 0; i < msgGalaxy.sectors.size(); i++)
            {
                const auto s = std::make_shared<Sector>();
                s->sector_id = msgGalaxy.sectors[i].id;
                s->position = msgGalaxy.sectors[i].position;
                sectors_map.insert(std::pair<int, std::shared_ptr<Sector>>(s->sector_id, s));
                sectors_vec.push_back(s);
            }

            // Setting neighbors sectors references
            for (int i = 0; i < msgGalaxy.sectors.size(); i++)
            {
                for (int j = 0; j < sectors_vec.size(); j++)
                {
                    if (msgGalaxy.sectors[i].id == sectors_vec[j]->sector_id)
                    {
                        for (int sec_idx = 0; sec_idx < msgGalaxy.sectors[i].neighbors_ids.size(); sec_idx++)
                        {
                            auto neighb = msgGalaxy.sectors[i].neighbors_ids;
                            sectors_vec[j]->neighbors.insert(sectors_map.at(neighb[sec_idx]));
                        }
                    }
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
                            SectorObject _sectorObject = SectorObject(star.id, star.position, star.object_size);
                            std::shared_ptr<Star> s_star = std::make_shared<Star>(_sectorObject, star.starType);
                            stars_map.insert(std::pair<int, std::shared_ptr<Star>>(star.id, s_star));
                            sectors_vec[j]->objects.insert(s_star);
                        }
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
                            SectorObject _sectorObject = SectorObject(planet.id, planet.position, planet.object_size);

                            std::map<PlanetaryFeatures::PlanetaryFeatureType, int> planetary_features;
                            for (int feat = 0; planet.planetary_features.size(); feat++)
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
                                colonies_map.insert(
                                    std::pair<int, std::shared_ptr<Colony>>(_planet->colony->id, _planet->colony));
                            }

                            _planet->possible_buildings = planet.possible_buildings;
                            planets_map.insert(std::pair<int, std::shared_ptr<Planet>>(planet.id, _planet));
                            sectors_vec[j]->objects.insert(_planet);
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
                            SectorObject _sectorObject =
                                SectorObject(inhabitable.id, inhabitable.position, inhabitable.object_size);
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
                            sectors_vec[j]->objects.insert(s_inhabitable);
                        }
                    }
                }
            }

            std::set<std::shared_ptr<Sector>> set_sectors;
            for (int i = 0; i < sectors_vec.size(); i++)
            {
                set_sectors.insert(sectors_vec[i]);
            }
            galaxy_ptr->sectors = set_sectors;

            // TODO: Add mutex
            auto state = context.getGameState();
            state.value->player = player_ptr;
        }
        break;

        case messageTypes::Actions:
        {
            auto payload_action = std::dynamic_pointer_cast<messageTypes::ActionsPayload>(des);
            std::vector<int> action = payload_action->createBaseActions;
        }
        break;

        case messageTypes::NewTurn:
        {
            auto payload_newturn = std::dynamic_pointer_cast<messageTypes::NewTurnPayload>(des);
            auto population_data = payload_newturn->updated_populations;
            auto resource_data = payload_newturn->updated_resources;
            auto state = context.getGameState();
            if (state.value->player != nullptr)
            {
                for (auto const& pop_data : population_data)
                {
                    if (state.value->player->owned_colonies.find(pop_data.first) !=
                        state.value->player->owned_colonies.end())
                    {
                        state.value->player->owned_colonies.at(pop_data.first)->population = pop_data.second;
                    }
                }

                if (resource_data.find(Resource::Metals) != resource_data.end())
                {
                    state.value->player->owned_resources.at(Resource::Metals) = resource_data.at(Resource::Metals);
                }

                if (resource_data.find(Resource::Antimatter) != resource_data.end())
                {
                    state.value->player->owned_resources.at(Resource::Antimatter) =
                        resource_data.at(Resource::Antimatter);
                }

                if (resource_data.find(Resource::RareMetals) != resource_data.end())
                {
                    state.value->player->owned_resources.at(Resource::RareMetals) =
                        resource_data.at(Resource::RareMetals);
                }

                if (resource_data.find(Resource::Crystals) != resource_data.end())
                {
                    state.value->player->owned_resources.at(Resource::Crystals) = resource_data.at(Resource::Crystals);
                }

                if (resource_data.find(Resource::Polymers) != resource_data.end())
                {
                    state.value->player->owned_resources.at(Resource::Polymers) = resource_data.at(Resource::Polymers);
                }

                if (resource_data.find(Resource::DarkMatter) != resource_data.end())
                {
                    state.value->player->owned_resources.at(Resource::DarkMatter) =
                        resource_data.at(Resource::DarkMatter);
                }

                if (resource_data.find(Resource::AncientNanobots) != resource_data.end())
                {
                    state.value->player->owned_resources.at(Resource::AncientNanobots) =
                        resource_data.at(Resource::AncientNanobots);
                }

                if (resource_data.find(Resource::AncientRelics) != resource_data.end())
                {
                    state.value->player->owned_resources.at(Resource::AncientRelics) =
                        resource_data.at(Resource::AncientRelics);
                }

                if (resource_data.find(Resource::Spatium) != resource_data.end())
                {
                    state.value->player->owned_resources.at(Resource::Spatium) = resource_data.at(Resource::Spatium);
                }

                if (resource_data.find(Resource::Food) != resource_data.end())
                {
                    state.value->player->owned_resources.at(Resource::Food) = resource_data.at(Resource::Food);
                }

                if (resource_data.find(Resource::Technology) != resource_data.end())
                {
                    state.value->player->owned_resources.at(Resource::Technology) =
                        resource_data.at(Resource::Technology);
                }
            }
        }
        break;
        case messageTypes::None:
            break;
    }

    auto abc = 7;
}

void uot_net_client::build_building(Building::BuildingType type)
{
    std::cout << "building building of type " << type << std::endl;
}
