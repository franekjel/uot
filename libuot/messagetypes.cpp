#include "messagetypes.h"

std::shared_ptr<messageTypes::BasePayload> messageTypes::Deserialize(std::string strMessage)
{
    nlohmann::json jsonMessage = nlohmann::json::parse(strMessage);
    Message message = jsonMessage.get<Message>();
    nlohmann::json jsonPayload = nlohmann::json::parse(message.payload);

    switch (message.messageType)
    {
        case MessageType::StartGame:
            return std::make_shared<StartGamePayload>(std::move(jsonPayload.get<StartGamePayload>()));
            break;
        case MessageType::AcceptJoin:
            return std::make_shared<AcceptJoinPayload>(std::move(jsonPayload.get<AcceptJoinPayload>()));
            break;
        case MessageType::Actions:
            return std::make_shared<ActionsPayload>(std::move(jsonPayload.get<ActionsPayload>()));
            break;
        case MessageType::NewTour:
            return std::make_shared<NewTourPayload>(std::move(jsonPayload.get<NewTourPayload>()));
            break;
    }

    return std::make_shared<BasePayload>();
}
