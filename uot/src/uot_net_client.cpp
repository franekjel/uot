#include "uot_net_client.h"
#include <array>
#include <iostream>

#include "../libuot/msg/messagetypes.h"

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
            // Galaxy gx = Galaxy(galaxy);
            std::vector<std::shared_ptr<Sector>> sectors_vec;
            for (int i = 0; i < msgGalaxy.sectors.size(); i++)
            {
                auto s = std::make_shared<Sector>();
                s->sector_id = msgGalaxy.sectors[i].id;
                s->position = msgGalaxy.sectors[i].position;
                sectors_vec.push_back(s);
            }

            for (int i = 0; i < sectors_vec.size(); i++)
            {
                for (int j = 0; j < msgGalaxy.sectors.size(); j++)
                {
                    if (sectors_vec[i]->sector_id == msgGalaxy.sectors[j].id)
                    {
                        for (int k = 0; k < msgGalaxy.sectors[j].neighbors_ids.size(); k++)
                        {
                            for (int l = 0; l < sectors_vec.size(); l++)
                            {
                                if (msgGalaxy.sectors[j].neighbors_ids[k] == sectors_vec[l]->sector_id)
                                {
                                    sectors_vec[i]->neighbors.insert(sectors_vec[l]);
                                }
                            }
                        }
                    }
                }
            }

            for (int i = 0; i < sectors_vec.size(); i++)
            {
                for (int j = 0; j < msgGalaxy.sectors.size(); j++)
                {
                    if (sectors_vec[i]->sector_id == msgGalaxy.sectors[j].id)
                    {
                        std::set<std::shared_ptr<SectorObject>> objects;
                        for (int p = 0; p < msgGalaxy.sectors[j].planets.size(); p++)
                        {
                            auto sectorobject =
                                SectorObject(sectors_vec[i]->sector_id, msgGalaxy.sectors[j].planets[p].position,
                                             msgGalaxy.sectors[j].planets[p].planet_size);
                            auto climate = msgGalaxy.sectors[j].planets[p].climate;
                            auto features = msgGalaxy.sectors[j].planets[p].planetary_features;
                            auto planet = std::make_shared<Planet>(sectorobject, climate, features);
                            objects.insert(planet);
                        }

                        /* for (int p = 0; p < msgGalaxy.sectors[j].stars.size(); p++)
                        {
                            messageTypes::MsgStar msgStar = msgGalaxy.sectors[j].stars[p];
                            auto s = std::make_shared<Star>(msgStar);
                            objects.insert(s);
                        }*/

                        /* for (int p = 0; p < msgGalaxy.sectors[j].inhabitables.size(); p++)
                        {
                            unsigned int id;
                            Point position;
                            float object_size;
                            std::map<Resource, float> resurce_deposit;
                            InhabitableObject::ObjectType object_type;
                            bool base_exists;
                            MsgBase base;
                            auto sectorobject =
                                SectorObject(sectors_vec[i]->sector_id, msgGalaxy.sectors[j].planets[p].position,
                                             msgGalaxy.sectors[j].planets[p].planet_size);
                            auto climate = msgGalaxy.sectors[j].planets[p].climate;
                            auto features = msgGalaxy.sectors[j].planets[p].planetary_features;
                            auto planet = std::make_shared<Planet>(sectorobject, climate, features);
                            objects.insert(planet);
                        }*/

                        sectors_vec[i]->objects = objects;
                    }
                }
            }

            /*for (int i = 0; i < sectors_vec.size(); i++)
            {
                sectors.insert(sectors_vec[i]);
            }*/

            /*std::vector<std::shared_ptr<Sector>> sectors;
            for (int i = 0; i < galaxy.sectors.size();i++)
            {
                auto s = std::make_shared<Sector>();
                s->sector_id = galaxy.sectors[i].id;
                s->position = galaxy.sectors[i].position;
                sectors.push_back(s);
            }
            for (int i = 0; i < sectors.size(); i++)
            {
                for (int j = 0; j < galaxy.sectors.size(); j++)
                {
                    if (sectors[i]->sector_id == galaxy.sectors[j].id)
                    {
                        for (int k = 0; k < galaxy.sectors[j].neighbors_ids.size(); k++)
                        {
                            for (int l = 0; l < sectors.size(); l++)
                            {
                                if (galaxy.sectors[j].neighbors_ids[k] == sectors[l]->sector_id)
                                {
                                    sectors[i]->neighbors.insert(sectors[l]);
                                }
                            }
                        }
                    }
                }
            }*/

            std::set<std::shared_ptr<Sector>> set_sectors;
            for (int i = 0; i < sectors_vec.size(); i++)
            {
                set_sectors.insert(sectors_vec[i]);
            }

            context.gs.galaxy->sectors = set_sectors;
        }
        break;

        case messageTypes::Actions:
        {
            auto payload_action = std::dynamic_pointer_cast<messageTypes::ActionsPayload>(des);
        }
        break;

        case messageTypes::NewTour:
        {
            auto payload_newtour = std::dynamic_pointer_cast<messageTypes::NewTourPayload>(des);
        }
        break;
    }

    auto abc = 7;
}
