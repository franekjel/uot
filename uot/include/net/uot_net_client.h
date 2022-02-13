#pragma once
#include <random>
#include <string>
#include <thread>

#include "buildings.h"
#include "client_context.h"
#include "msg/messagestructs.h"
#include "msg/messagetypes.h"
#include "net/client_txrx.h"

struct uot_net_client : net_client
{
    uot_net_client(client_context& cc);
    client_context& context;
    std::string name;
    sns_client_txrx txrx;
    bool getting_input;
    bool disconnected;
    std::thread player_payload;

    void input_thread();
    void run();
    void connect_to_server();
    void disconnect_from_server();
    void disconnect();
    std::string get_name() override;
    void handle_status_change(net_status status) override;
    void handle_message(const std::string& data) override;

    void send_payload();
    void updateFleet(const std::shared_ptr<Fleet> f, const messageTypes::MsgFleetParameters p);

    void parseSector(const messageTypes::MsgSector& sec_msg);
    void parseBuildingsUpdates(std::shared_ptr<game_state>& state,
                               const std::vector<messageTypes::MsgBuildingsUpdates>& buildings_updates);
    void parseShipsUpdates(std::shared_ptr<game_state>& state,
                           const std::vector<messageTypes::MsgShipUpdate>& ships_updates);

    void parseTechnologyUpdate(std::shared_ptr<game_state>& state,
                               const std::vector<messageTypes::MsgTechnologyUpdate>& technology_updates);

    void parseNewShipsUpdate(std::shared_ptr<game_state>& state,
                             const std::vector<messageTypes::MsgNewShip>& new_ships);

    void parseWatchedSectorUpdate(std::shared_ptr<game_state>& state,
                                  const std::vector<messageTypes::MsgWatchedSectorUpdate>& watched_sectors);
    void parseDestroyedShips(std::shared_ptr<game_state>& state, const std::vector<unsigned int>& destroyed_ships);

    void parseDesigns(std::shared_ptr<game_state>& state,
                      const std::vector<messageTypes::MsgShipDesignResponse>& ship_designs);

    void parseFleetInfo(std::shared_ptr<Fleet> f, const messageTypes::MsgDetailedFleetInfo& i);

    void parseFleetsPopulationChanges(std::shared_ptr<game_state>& state,
                                      const std::vector<messageTypes::MsgChangedFleetPopulation>& update);

    void parseJoinedFleets(std::shared_ptr<game_state>& state,
                           const std::vector<messageTypes::MsgFleetsJoin>& joined_fleets);

    void parseJumpedFleets(std::shared_ptr<game_state>& state,
                           const std::vector<messageTypes::MsgFleetsJump>& jumped_fleets);

    void parseLostObjects(std::shared_ptr<game_state>& state, const std::vector<unsigned int>& lost);
    void parseInvadedColonies(std::shared_ptr<game_state>& state, const std::vector<messageTypes::MsgColony>& invaded);

    void updateAnimations(std::shared_ptr<game_state>& state,
                          const std::vector<messageTypes::MsgFleetParameters>& fleets_in_fight);

    // not best place for this but I don't have better idea
    std::random_device dev;
    std::mt19937 gen;
};
