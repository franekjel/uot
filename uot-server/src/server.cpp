#include "../headers/server.h"

Server::Server() : players(), messaging_service(), game_in_progress(false)
{
    messaging_service.set_accept_player_callback([&](std::string player_name)
                                                 { return this->accept_player(player_name); });
    messaging_service.set_after_accept_player_callback([&]()
                                                 { this->after_accept_player(); });
    messaging_thread = std::thread(&net_server_uot::run, &messaging_service);
    galaxy = std::make_shared<Galaxy>(GenerateGalaxy({galaxy_size, galaxy_habitable_planet_chance_multipler}));
    messaging_thread.join();
}

void Server::StartGame()
{
    game_in_progress = true;
    // TODO: Send players info about beginning of the game
    players.SendStartGameMessage(messaging_service);
    run();
}

void Server::run()
{
    while (game_in_progress)
    {
        {
            std::lock_guard<std::mutex> locking_numbers(counting_numbers);
            players.CountEveryTourNumbers();
            if (++tour_number % WEEK_LENGTH == 0)
                players.CountWeeklyNumbers();
            players.SendNewTourMessage(tour_number, messaging_service);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        std::cout << "dupa, dzialam se juz " << tour_number << " tur\n";
    }
}

bool Server::accept_player(std::string player_name)
{
    if (!game_in_progress)
    {
        return players.AddPlayer(player_name, galaxy);
    }
    return false;
}


void Server::after_accept_player()
{
    if (players_count_game_start == PlayersCount())
        StartGame();
}
