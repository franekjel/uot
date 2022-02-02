#ifndef UOT_MSG_QUEUE_H
#define UOT_MSG_QUEUE_H
#include "msg/messagetypes.h"

struct msg_queue
{
    messageTypes::ActionsPayload actions;
    void build_building(int colony_id, Building::BuildingType type);
    void upgrade_building(int colony_id, Building::BuildingType type_from, Building::BuildingType type_to);
};

#endif  // UOT_MSG_QUEUE_H
