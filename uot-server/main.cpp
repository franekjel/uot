#include "headers/server.h"

#include <fstream>
#include <iostream>
#include <sstream>

void StoreValue(Server& server, std::string key, std::string value)
{
    if (key == "start_players_count")
    {
        server.start_players_count = atoi(value.c_str());
    }
}

void ReadConfig(Server& server)
{
    std::ifstream file("config.txt");

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream is_line(line);
        std::string key;
        if (std::getline(is_line, key, '='))
        {
            std::string value;
            if (std::getline(is_line, value))
            {
                StoreValue(server, key, value);
            }
        }
    }
}

int main()
{
    bool running = true;
    Server s = Server();

    ReadConfig(s);

    while (true)
    {
        char command;
        std::cin >> command;
        if (command == 'e')
            running = false;
        else if (command == 's')
            s.StartGame();
        else
            std::cout << "Unknown command " << std::string(&command, 1);

        if (s.start_players_count == s.PlayersCount())
            s.StartGame();
    }

    // PlayersList players_list = {};
    // players_list.AddPlayer(std::make_shared<Galaxy>());
    return 0;
}
