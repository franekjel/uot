#pragma once
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>

namespace messageTypes
{
enum MessageType
{
    None,
    StartGame,
    AcceptJoin
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
    int jakiesPole1;
    std::string jakiesPole2;
    MessageType GetType() override { return MessageType::StartGame; }
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(StartGamePayload, jakiesPole1, jakiesPole2)
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

struct AcceptJoinPayload : BasePayload
{
    bool ok;
    MessageType GetType() override { return MessageType::AcceptJoin; }
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(AcceptJoinPayload, ok)
    std::string Serialize() override
    {
        nlohmann::json jsonPayload = (*this);
        Message message;
        message.messageType = MessageType::AcceptJoin;
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