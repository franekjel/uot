#include "players.h"
int main()
{
    PlayersList players_list = {};
    players_list.AddPlayer(std::make_shared<Galaxy>());
    return 0;
}
