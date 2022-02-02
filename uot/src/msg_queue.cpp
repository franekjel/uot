#include "msg_queue.h"
#include <iostream>

void msg_queue::build_building(int colony_id, Building::BuildingType type)
{
    messageTypes::MsgBuildRequest request{colony_id, type, Building::BuildingType::None};
    std::cout << "building building of type " << type << std::endl;
    actions.buildRequests.push_back(request);
}

void msg_queue::upgrade_building(int colony_id, Building::BuildingType type_from, Building::BuildingType type_to)
{
    messageTypes::MsgBuildRequest request{colony_id, type_to, type_from};
    std::cout << "upgrading building of type " << type_from << " to " << type_to << std::endl;
    actions.buildRequests.push_back(request);
}