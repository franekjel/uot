#pragma once
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include "galaxy.h"
#include "messagestructs.h"
#include "player.h"
#include "resource.h"

/*
How to add new message
    1. Add new MessageType to enum
    2. Create new struct inheriting BasePayload
    3. Add properties - should have at least one
    4. Use NLOHMANN_DEFINE_TYPE_INTRUSIVE macro
    5. Implement GetType and Serialize (copy-paste with enum replacing)
    6. Add new message to Deserialize (copy-paste with enum replacing)
*/

namespace messageTypes
{
enum MessageType
{
    None,
    StartGame,
    Actions,
    NewTurn
};

struct Message
{
    MessageType messageType;
    std::string payload;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Message, messageType, payload)
};

struct BasePayload
{
    virtual MessageType GetType() { return MessageType::None; };
    virtual std::string Serialize() { return ""; };
};

struct StartGamePayload : BasePayload
{
    unsigned int player_id;
    MsgGalaxy galaxy;
    std::map<Resource, float> starting_resources; /*resource, amount*/
    std::vector<MsgShipDesignResponse> starting_ships_designs;
    MessageType GetType() override { return MessageType::StartGame; }
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(StartGamePayload, player_id, galaxy, starting_resources, starting_ships_designs)
    std::string Serialize() override
    {
        nlohmann::json jsonPayload = (*this);
        Message message;
        message.messageType = MessageType::StartGame;
        message.payload = jsonPayload.dump();
        nlohmann::json jsonMessage = message;
        return jsonMessage.dump();
    }
};

struct ActionsPayload : BasePayload  // Player's actions
{
    std::vector<MsgBuildRequest> buildRequests;
    std::vector<MsgMoveFleetRequest> moveFleetRequests;
    Technology::TechnologyType technologyRequest;  // if none requested, set to Empty
    std::vector<MsgJoinFleetsRequest> joinFleetsRequests;
    std::vector<MsgFleetActionRequest> fleetActionRequests;
    std::vector<MsgShipDesign> shipDesignRequests;
    std::vector<MsgCreateShip> createShipRequests;
    std::vector<unsigned int> fleetInfoRequests;
    MessageType GetType() override { return MessageType::Actions; }
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ActionsPayload, technologyRequest, buildRequests, moveFleetRequests,
                                   joinFleetsRequests, fleetActionRequests, shipDesignRequests, createShipRequests,
                                   fleetInfoRequests)
    std::string Serialize() override
    {
        nlohmann::json jsonPayload = (*this);
        Message message;
        message.messageType = MessageType::Actions;
        message.payload = jsonPayload.dump();
        nlohmann::json jsonMessage = message;
        return jsonMessage.dump();
    }
};

struct NewTurnPayload : BasePayload  // New turn
{
    std::map<Resource, float> updated_resources; /*resource, amount*/
    std::map<int, float> updated_populations;    /*colony_id, population*/
    std::map<int, float> updated_soldiers;       /*colony_id, soldiers*/
    std::vector<MsgTechnologyUpdate> technology_updates;
    std::vector<MsgBuildingsUpdates> buildings_updates;
    std::vector<MsgWatchedSectorUpdate> watched_sectors_updates;
    std::vector<MsgSector> new_sectors;
    std::vector<MsgFleetsJoin> joined_fleets;
    std::vector<MsgFleetsJump> jumped_fleets;
    std::vector<MsgShipDesignResponse> ship_designs;
    std::vector<MsgShipUpdate> ships_updates;
    std::vector<MsgNewShip> new_ships;
    std::vector<MsgFleetParameters> fleets_in_fight;
    std::vector<MsgChangedFleetPopulation> changed_fleet_populations;
    std::vector<unsigned int> destroyed_ships;
    std::vector<unsigned int> lost_objects;  // colony on planet or base on inhabitable
    std::vector<MsgColony> invaded_colonies;
    std::vector<MsgDetailedFleetInfo> fleet_info_response;
    MessageType GetType() override { return MessageType::NewTurn; }
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(NewTurnPayload, updated_resources, updated_populations, updated_soldiers,
                                   technology_updates, buildings_updates, watched_sectors_updates, new_sectors,
                                   joined_fleets, jumped_fleets, ship_designs, ships_updates, new_ships,
                                   fleets_in_fight, changed_fleet_populations, destroyed_ships, lost_objects,
                                   invaded_colonies, fleet_info_response)
    std::string Serialize() override
    {
        nlohmann::json jsonPayload = (*this);
        Message message;
        message.messageType = MessageType::NewTurn;
        message.payload = jsonPayload.dump();
        nlohmann::json jsonMessage = message;
        return jsonMessage.dump();
    }
};

std::shared_ptr<BasePayload> Deserialize(std::string strMessage);

// SAMPLE:
/*
    messageTypes::StartGamePayload sgp;
    sgp.jakiesPole1 = 12;
    sgp.jakiesPole2 = "33";
    auto ser = sgp.Serialize();
    std::shared_ptr<messageTypes::BasePayload> des = messageTypes::Deserialize(ser);
    auto type = des->GetType();
    auto cast = std::dynamic_pointer_cast<messageTypes::StartGamePayload>(des);
*/

}  // namespace messageTypes
