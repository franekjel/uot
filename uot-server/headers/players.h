#pragma once
#include <memory>
#include <stdexcept>
#include <thread>
#include <vector>

#include "../net/net_server_uot.h"
#include "galaxy.h"
#include "player.h"

static long player_id = 1;
class PlayersList
{
   private:
    std::map<long, std::shared_ptr<Player>> players = {};
    std::map<long, std::string> players_net_names = {};

   public:
    std::shared_ptr<Galaxy> GetStartingGalaxy(std::shared_ptr<Galaxy> wholeGalaxy);
    std::map<Resource, float> GetStartingResources();
    std::shared_ptr<Colony> GetStartingColony(long player_id, std::shared_ptr<Galaxy> startingGalaxy,
                                              std::shared_ptr<Galaxy> wholeGalaxy);
    void AddPlayer(std::string player_net_name, std::shared_ptr<Galaxy> wholeGalaxy);
    void CountWeeklyNumbers();
    void CountEveryTourNumbers();
    int PlayersCount() { return players.size(); }
    void SendNewTourMessage(int tour_number, net_server_uot& messaging_service);
    void SendStartGameMessage(net_server_uot& messaging_service);
    static void CountWeeklyNumbersPlayer(std::shared_ptr<Player> player);
    static void CountEveryTourNumbersPlayer(std::shared_ptr<Player> player);
};
