#pragma once
#include <memory>
#include <stdexcept>
#include <thread>
#include <vector>

#include "../net/net_server_uot.h"
#include "galaxy.h"
#include "player.h"

static unsigned int player_id = 1;
class PlayersList
{
   private:
    std::map<unsigned int, std::shared_ptr<Player>> players = {};
    std::map<unsigned int, std::string> players_net_names = {};
    std::map<std::string, unsigned int> players_net_names_rev = {};

   public:
    std::shared_ptr<Galaxy> GetStartingGalaxy(std::shared_ptr<Galaxy> wholeGalaxy);
    std::map<Resource, float> GetStartingResources();
    std::shared_ptr<Colony> GetStartingColony(unsigned int player_id, std::shared_ptr<Galaxy> startingGalaxy,
                                              std::shared_ptr<Galaxy> wholeGalaxy);
    bool AddPlayer(std::string player_net_name, std::shared_ptr<Galaxy> wholeGalaxy);
    bool HandlePlayerRequests(std::string player_net_name, std::shared_ptr<messageTypes::ActionsPayload>);
    void CountWeeklyNumbers();
    void CountEveryTurnNumbers();
    int PlayersCount() { return players.size(); }
    void SendNewTurnMessage(int turn_number, net_server_uot& messaging_service, std::shared_ptr<Galaxy>& galaxy);
    void SendStartGameMessage(net_server_uot& messaging_service);
    static void CountWeeklyNumbersPlayer(std::shared_ptr<Player> player);
    static void CountEveryTurnNumbersPlayer(std::shared_ptr<Player> player);
};
