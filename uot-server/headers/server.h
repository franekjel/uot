#pragma once
#include "../net/net_server_uot.h"
#include "galaxygenerator.h"
#include "players.h"

#include <iostream>
#include <mutex>
#include <thread>

#define WEEK_LENGTH 2

class Server
{
   public:
    Server();

    void StartGame();
    int PlayersCount() { return players.PlayersCount(); }
    int players_count_game_start = -1;

   private:
    bool accept_player(std::string player_name);
    bool handle_player_actions(std::string player_name, std::shared_ptr<messageTypes::ActionsPayload> payload);
    void after_accept_player();
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