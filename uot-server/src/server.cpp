#include "..\headers\server.h"

Server::Server() : players(), messaging_service(), game_in_progress(false)
{
    messaging_service.set_accept_player_callback([&](std::string player_name) { this->accept_player(player_name); });
    messaging_thread = std::thread(&net_server_uot::run, &messaging_service);
    galaxy = std::make_shared<Galaxy>(GenerateGalaxy({galaxy_size, galaxy_habitable_planet_chance_multipler}));
}

Server::~Server() { messaging_thread.join(); }

void Server::StartGame()
{
    game_in_progress = true;
    // TODO: Send players info about beginning of the game
    messaging_service.send_game_begin_message();
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
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        std::cout << "dupa, dzialam se juz " << tour_number << " tur\n";
    }
}

void Server::accept_player(std::string player_name)
{
    if (!game_in_progress)
    {
        players.AddPlayer(player_name, galaxy);
    }
}
