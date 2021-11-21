#pragma once
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>

enum MessageType
{
    None,
    StartMessage
};

struct Message
{
    MessageType messageType;
    std::string payload;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Message, messageType, payload)
};

struct BasePayload
{
    virtual MessageType GetType() { return MessageType::StartMessage; };
    virtual std::string Serialize() { return ""; };
};

struct StartGamePayload : BasePayload
{
    int jakiesPole1;
    std::string jakiesPole2;
    MessageType GetType() override { return MessageType::StartMessage; }
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(StartGamePayload, jakiesPole1, jakiesPole2)
    std::string Serialize() override
    {
        nlohmann::json jsonPayload = (*this);
        Message message;
        message.messageType = MessageType::StartMessage;
        message.payload = jsonPayload.dump();
        nlohmann::json jsonMessage = message;
        return jsonMessage.dump();
    }
};

std::shared_ptr<BasePayload> Deserialize(std::string strMessage)
{
    nlohmann::json jsonMessage = nlohmann::json::parse(strMessage);
    Message message = jsonMessage.get<Message>();
    nlohmann::json jsonPayload = nlohmann::json::parse(message.payload);

    switch (message.messageType)
    {
        case MessageType::StartMessage:
            return std::make_shared<StartGamePayload>(std::move(jsonPayload.get<StartGamePayload>()));
            break;
    }

    return {};
}

//SAMPLE:
/*
    StartGamePayload sgp;
    sgp.jakiesPole1 = 12;
    sgp.jakiesPole2 = "33";
    auto ser = sgp.Serialize();
    std::shared_ptr<BasePayload> des = Deserialize(ser);
    auto type = des->GetType();
    auto cast = std::dynamic_pointer_cast<StartGamePayload>(des);
*/