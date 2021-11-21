#pragma once
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include
enum MessageType
{
    StartMessage
};
struct Message
{
    MessageType messageType;
    std::string payload;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Message, messageType, payload)

struct BasePayload
{
    virtual std::string Serialize() = 0;
};

struct StartGamePayload : BasePayload
{
    int jakiesPole1;
    std::string jakiesPole2;
    std::string Serialize() override
    {
        nlohmann::json jsonPayoload = this;
        Message message;
        message.messageType = MessageType::StartMessage;
        message.payload = jsonPayoload.dump();
        nlohmann::json jsonMessage;
        return jsonMessage.dump();
    }
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(StartGamePayload, jakiesPole1, jakiesPole2)


BasePayload Deserialize(std::string strMessage) { 
    nlohmann::json jsonMessage = nlohmann::json::parse(strMessage);
    Message message = jsonMessage.get<Message>();
    nlohmann::json jsonPayload = nlohmann::json::parse(message.payload);

    switch (message.messageType)
    {
        case MessageType::StartMessage:
            return jsonPayload.get<StartGamePayload>();
            break;
    }

    return BasePayload;
}