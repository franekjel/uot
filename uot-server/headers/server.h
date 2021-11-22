#pragma once
#include "../net/net_server_uot.h"
#include "galaxygenerator.h"
#include "players.h"

#include <iostream>
#include <mutex>
#include <thread>

class Server
{
   public:
    Server();
    ~Server();

    void StartGame();
    int PlayersCount() { return players.PlayersCount(); }
    int start_players_count = -1;
   private:
    void accept_player();
    void run();

    PlayersList players;
    net_server_uot messaging_service;
    bool game_in_progress;
    std::shared_ptr<Galaxy> galaxy;

    std::thread messaging_thread;

    std::mutex counting_numbers;

    int tour_number = 0;

    const int galaxy_size = 10;
    const float galaxy_habitable_planet_chance_multipler = 10.5f;
};