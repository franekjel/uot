#include "net_server_uot.h"
#include <player.h>
#include <iostream>

#define INF_TIME 999999
#define EPS 0.0001f

net_server_uot::net_server_uot() : txrx(*this, 40), running(false) { txrx.set_as_handler(); }

bool net_server_uot::accept_player(const server_txrx::net_player& player)
{
    std::cout << "Player " << player.name << " wants to connect with address " << player.IP << "\n";
    auto res = uot_accept_player(player.name);
    if (res)
        players.push_back(player.name);
    return res;
}

void net_server_uot::after_accept_player() { uot_after_accept_player(); }

void net_server_uot::handle_status_change(const std::string& player_name, net_server::net_status status)
{
    if (status == net_server::net_status::disconnect)
    {
        std::cout << "Player " << player_name << " disconnected\n";
        for (int i = 0; i < players.size(); i++)
        {
            if (players[i] == player_name)
            {
                players.erase(players.begin() + i);
                break;
            }
        }
    }
}
void net_server_uot::handle_message(const std::string& player_name, const std::string& data)
{
    std::cout << player_name << ": " << data << "\n";

    auto deserialized = messageTypes::Deserialize(data);
    if (!!deserialized)
    {
        if (deserialized->GetType() == messageTypes::MessageType::Actions)
        {
            auto cast = std::dynamic_pointer_cast<messageTypes::ActionsPayload>(deserialized);
            uot_handle_actions(player_name, cast);
        }
    }
}

void net_server_uot::run()
{
    uint16 port_num = 7645;
    txrx.start_socket(port_num);
    std::cout << "Server listening on port " << port_num;
    running = true;
}

void net_server_uot::set_accept_player_callback(std::function<bool(std::string player_name)> callback)
{
    uot_accept_player = callback;
}

void net_server_uot::set_handle_actions_callback(
    std::function<void(std::string, std::shared_ptr<messageTypes::ActionsPayload>)> callback)
{
    uot_handle_actions = callback;
}

void net_server_uot::set_after_accept_player_callback(std::function<void()> callback)
{
    uot_after_accept_player = callback;
}

void net_server_uot::send_new_turn_message(int turn_number, std::shared_ptr<Player>& player,
                                           std::string player_net_name, std::shared_ptr<Galaxy>& galaxy)
{
    messageTypes::NewTurnPayload payload;

    for (const auto& resource : player->owned_resources)
    {
        if (player->resources_changed[resource.first])
        {
            payload.updated_resources[resource.first] = resource.second;
            player->resources_changed[resource.first] = false;
        }
    }

    for (const auto& colony : player->owned_colonies)
    {
        if (colony.second->population_changed)
        {
            payload.updated_populations[colony.second->id] = colony.second->population;
            colony.second->population_changed = false;
        }

        if (colony.second->building_queue_changed)
        {
            float work_offset = 0.0f;
            float divider = colony.second->population_building_modificator * colony.second->unemployed_population;

            for (const auto& building : colony.second->building_queue)
            {
                
                if (divider > EPS)
                    work_offset += building.worker_week_units_left / divider;
                else
                    work_offset = INF_TIME;
                payload.buildings_updates.push_back(messageTypes::MsgBuildingsUpdates(
                    colony.second->id, building.type, building.upgrade_of, work_offset));
            }
            colony.second->building_queue_changed = false;
        }
    }

    for (const auto& sector : galaxy->sectors)
    {
        if (sector->watchers.count(player->id) != 0)
        {
            auto sector_update_msg = messageTypes::MsgWatchedSectorUpdate(sector->sector_id);
            for (const auto& fleet : sector->present_fleets)
            {
                sector_update_msg.fleets.push_back(messageTypes::MsgFleet(fleet, fleet->owner_id));
            }
        }
    }

    txrx.send_reliable(player_net_name, payload.Serialize());
}

void net_server_uot::send_game_begin_message(std::shared_ptr<Player>& player, std::string player_net_name)
{
    messageTypes::StartGamePayload payload;

    payload.player_id = player->id;
    payload.galaxy = messageTypes::MsgGalaxy(player->known_galaxy);
    payload.starting_resources = {};
    for (const auto& resource : player->owned_resources)
    {
        payload.starting_resources[resource.first] = resource.second;
    }

    txrx.send_reliable(player_net_name, payload.Serialize());
}

void net_server_uot::read_input()
{
    while (running)
    {
        char command;
        std::cin >> command;
        if (command == 'e')
            running = false;
        else
            std::cout << "Unknown command " << std::string(&command, 1);
    }
}
