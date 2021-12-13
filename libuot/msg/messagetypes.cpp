#include "messagetypes.h"

std::shared_ptr<messageTypes::BasePayload> messageTypes::Deserialize(std::string strMessage)
{
    nlohmann::json jsonMessage = nlohmann::json::parse(strMessage, nullptr, false);
    if (jsonMessage.is_discarded())
        return {};
    Message message = jsonMessage.get<Message>();
    nlohmann::json jsonPayload = nlohmann::json::parse(message.payload);

    switch (message.messageType)
    {
        case MessageType::StartGame:
            return std::make_shared<StartGamePayload>(std::move(jsonPayload.get<StartGamePayload>()));
            break;
        case MessageType::Actions:
            return std::make_shared<ActionsPayload>(std::move(jsonPayload.get<ActionsPayload>()));
            break;
        case MessageType::NewTurn:
            return std::make_shared<NewTurnPayload>(std::move(jsonPayload.get<NewTurnPayload>()));
            break;
    }

    return std::make_shared<BasePayload>();
}
