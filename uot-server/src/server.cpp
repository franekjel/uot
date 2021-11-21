#include "..\headers\server.h"

Server::Server() : players(), messaging_service(), game_in_progress(false)
{
    messaging_service.set_accept_player_callback([&]() { this->accept_player(); });
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
            players.CountNumbers();
            messaging_service.send_new_tour_message(++tour_number);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        std::cout << "dupa, dzialam se juz " << tour_number << " tur\n";
    }
}

void Server::accept_player()
{
    if (!game_in_progress)
        players.AddPlayer(galaxy);
}
