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
    MessageType GetType() override { return MessageType::StartGame; }
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(StartGamePayload, player_id, galaxy)
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
    /*TBD*/
    // std::vector<int> createColonyActions; /*objectId*/
    // std::vector<int> createBaseActions; /*objectId*/
    std::vector<MsgBuildRequest> buildRequests;
    std::vector<MsgMoveFleetRequest> moveFleetRequests;
    std::vector<int> createBaseActions;            /*objectId*/
    Technology::TechnologyType technologyRequest;  // if none requested, set to None
    MessageType GetType() override { return MessageType::Actions; }
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ActionsPayload, createBaseActions, technologyRequest, buildRequests,
                                   moveFleetRequests)
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
    std::map<int, int> updated_populations;      /*colony_id, population*/
    MsgTechnologyUpdate technology_update;
    std::vector<MsgBuildingsUpdates> buildings_updates;
    MessageType GetType() override { return MessageType::NewTurn; }
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(NewTurnPayload, updated_resources, updated_populations, buildings_updates,
                                   technology_update)
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