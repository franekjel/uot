#ifndef UOT_MSG_QUEUE_H
#define UOT_MSG_QUEUE_H
#include "msg/messagetypes.h"

struct msg_queue
{
    messageTypes::ActionsPayload actions;
    void build_building(int colony_id, Building::BuildingType type);
    void upgrade_building(int colony_id, Building::BuildingType type_from, Building::BuildingType type_to);
    void request_research(Technology::TechnologyType tech);
    void request_fleet_move(unsigned int fleet_id, const Point pos);
    void reset();
};

#endif  // UOT_MSG_QUEUE_H
