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
    void build_design(std::shared_ptr<ShipDesign>& design, const bool _delete);
    void build_ship(const int design_id, const int planet_id);
    void request_fleet_jump(unsigned int fleet_id);
    void request_fleet_build(unsigned int fleet_id);
    void request_fleet_colonize(unsigned int fleet_id);
    void request_fleet_civil_on(unsigned int fleet_id);
    void request_fleet_civil_off(unsigned int fleet_id);
    void request_fleet_soldiers_on(unsigned int fleet_id);
    void request_fleet_soldiers_off(unsigned int fleet_id);
    void request_fleet_invade(unsigned int fleet_id);
    void request_fleet_cancel(unsigned int fleet_id);
    void reset();
};

#endif  // UOT_MSG_QUEUE_H
