#include "net_server_mock.h"
#include <synchapi.h>
#include <iostream>
using namespace std;

//  TODO : copy dll/exes when building

int main(void)
{
    cout << "Starting...\n";
    net_server_mock server;
    server.run();
}

net_server_mock::net_server_mock() : txrx(*this, 40), running(false) { txrx.set_as_handler(); }

bool net_server_mock::accept_player(const server_txrx::net_player& player)
{
    cout << "Player " << player.name << " wants to connect with address " << player.IP << "\n";
    players.push_back(player.name);
    return true;
}

void net_server_mock::handle_status_change(const string& player_name, net_server::net_status status)
{
    if (status == net_server::net_status::disconnect)
    {
        cout << "Player " << player_name << " disconnected\n";
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
void net_server_mock::handle_message(const string& player_name, const string& data)
{
    cout << player_name << ": " << data << "\n";
    for (auto& s : players)
    {
        txrx.send_reliable(s, data);
    }
}

void net_server_mock::run()
{
    uint16 port_num = 7645;
    txrx.start_socket(port_num);
    cout << "Server listening on port " << port_num;
    running = true;
    read_thread = thread(&net_server_mock::read_input, this);

    while (running)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        string send_str = "Server is up";
        for (auto& s : players)
        {
            txrx.send_reliable(s, send_str);
        }
    }

    read_thread.join();
}
void net_server_mock::read_input()
{
    while (running)
    {
        char command;
        cin >> command;
        if (command == 'e')
            running = false;
        else
            cout << "Unknown command " << string(&command, 1);
    }
}
