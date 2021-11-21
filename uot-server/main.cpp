#include "headers/server.h"

#include <iostream>

int main()
{
    bool running = true;
    Server s = Server();

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
    }

    //PlayersList players_list = {};
    //players_list.AddPlayer(std::make_shared<Galaxy>());
    return 0;
}
