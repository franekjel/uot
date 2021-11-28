#include <iostream>
#include "messagetypes.h"

void NewTourPayloadTest()
{
    messageTypes::NewTourPayload ntp;

    float food_init = 100.0, metals_init = 10.0;
    float p1_init = 100.0, p10_init = 10.0;
    ntp.updated_resources.insert(std::pair<Resource, int>(Resource::Food, food_init));
    ntp.updated_resources.insert(std::pair<Resource, int>(Resource::Metals, metals_init));

    ntp.updated_populations.insert(std::pair<int, int>(1, p1_init));
    ntp.updated_populations.insert(std::pair<int, int>(10, p10_init));

    auto ser = ntp.Serialize();
    std::shared_ptr<messageTypes::BasePayload> des = messageTypes::Deserialize(ser);
    auto type = des->GetType();
    if (type != messageTypes::MessageType::NewTour)
    {
        std::cout << "NewTour - wrong message type\n";
    }
    auto cast = std::dynamic_pointer_cast<messageTypes::NewTourPayload>(des);

    if (cast->updated_resources.size() != 2)
    {
        std::cout << "NewTour - wrong resources size\n";
    }

    if (cast->updated_populations.size() != 2)
    {
        std::cout << "NewTour - wrong populations size\n";
    }

    auto food = cast->updated_resources[Resource::Food];
    auto metals = cast->updated_resources[Resource::Metals];
    if (food != food_init || metals != metals_init)
    {
        std::cout << "NewTour - wrong resource values\n";
    }

    auto p1 = cast->updated_populations[1];
    auto p10 = cast->updated_populations[10];
    if (p1 != p1_init || p10 != p10_init)
    {
        std::cout << "NewTour - wrong people values\n";
    }
}

void ActionsPayloadTest()
{
    messageTypes::ActionsPayload ap;

    auto ser = ap.Serialize();
    std::shared_ptr<messageTypes::BasePayload> des = messageTypes::Deserialize(ser);
    auto type = des->GetType();
    if (type != messageTypes::MessageType::Actions)
    {
        std::cout << "Actions - wrong message type\n";
    }
    auto cast = std::dynamic_pointer_cast<messageTypes::ActionsPayload>(des);

    if (cast->createBaseActions.size() != 0)
    {
        std::cout << "Actions - wrong resources size\n";
    }
}

int main()
{
    std::cout << "Tests started\n";

    NewTourPayloadTest();
    ActionsPayloadTest();

    std::cout << "Tests finished\n";
    std::cin.get();

    return 0;
}