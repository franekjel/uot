#include "net_server_uot.h"
#include <player.h>
#include <iostream>

net_server_uot::net_server_uot() : txrx(*this, 40), running(false) { txrx.set_as_handler(); }

bool net_server_uot::accept_player(const server_txrx::net_player& player)
{
    std::cout << "Player " << player.name << " wants to connect with address " << player.IP << "\n";
    players.push_back(player.name);
    uot_accept_player(player.name);
    return true;
}

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
    for (auto& s : players)
    {
        txrx.send_reliable(s, data);
    }
}

void net_server_uot::run()
{
    uint16 port_num = 7645;
    txrx.start_socket(port_num);
    std::cout << "Server listening on port " << port_num;
    running = true;
}

void net_server_uot::set_accept_player_callback(std::function<void(std::string player_name)> callback)
{
    uot_accept_player = callback;
}
void net_server_uot::send_new_tour_message(int tour_number, std::shared_ptr<Player>& player,
                                           std::string player_net_name)
{
    messageTypes::NewTourPayload payload;

    for (auto& resource : player->owned_resources)
    {
        if (player->resources_changed[resource.first])
        {
            payload.updated_resources[resource.first] = resource.second;
            player->resources_changed[resource.first] = false;
        }
    }

    for (auto& colony : player->owned_colonies)
    {
        if (colony->population_changed)
        {
            payload.updated_populations[colony->id] = colony->population;
            colony->population_changed = false;
        }
    }

    txrx.send_reliable(player_net_name, payload.Serialize());
}

void net_server_uot::send_game_begin_message()
{
    for (auto& s : players)
    {
        txrx.send_reliable(s, "blabla");  // TODO change to propper message, when messaging will be done
    }
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
