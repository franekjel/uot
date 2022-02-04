#include "../headers/server.h"

Server::Server() : players(), messaging_service(), game_in_progress(false)
{
    messaging_service.set_accept_player_callback([&](std::string player_name)
                                                 { return this->accept_player(player_name); });
    messaging_service.set_handle_actions_callback(
        [&](std::string player_name, std::shared_ptr<messageTypes::ActionsPayload> actions)
        { this->handle_player_actions(player_name, actions); });
    messaging_service.set_after_accept_player_callback([&]() { this->after_accept_player(); });
    messaging_thread = std::thread(&net_server_uot::run, &messaging_service);
    GalaxyGenerator generator(galaxy_size, galaxy_habitable_planet_chance_multipler);
    galaxy = std::make_shared<Galaxy>(generator.Generate());
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
            players.CountEveryTurnNumbers();
            if (++turn_number % WEEK_LENGTH == 0)
                players.CountWeeklyNumbers();
            players.SendNewTurnMessage(turn_number, messaging_service, galaxy);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        std::cout << "rozpoczyna sie tura: " << turn_number << "\n";
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

bool Server::handle_player_actions(std::string player_name, std::shared_ptr<messageTypes::ActionsPayload> payload)
{
    if (!game_in_progress)
    {
        return false;
    }

    return players.HandlePlayerRequests(player_name, payload);
}

void Server::after_accept_player()
{
    if (players_count_game_start == PlayersCount())
        StartGame();
}
