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

void msg_queue::request_research(Technology::TechnologyType tech) { actions.technologyRequest = tech; }

void msg_queue::request_fleet_move(const unsigned int fleet_id, const Point pos)
{
    messageTypes::MsgMoveFleetRequest request(fleet_id, pos);
    actions.moveFleetRequests.push_back(request);
}

void msg_queue::reset()
{
    actions.buildRequests.clear();
    actions.moveFleetRequests.clear();
    actions.technologyRequest = Technology::TechnologyType::Empty;
    actions.fleetActionRequests.clear();
    actions.shipDesignRequests.clear();
    actions.createShipRequests.clear();
}

void msg_queue::build_ship(const int design_id, const int planet_id)
{
    messageTypes::MsgCreateShip request(design_id, planet_id);
    actions.createShipRequests.push_back(request);
}

void msg_queue::build_design(std::shared_ptr<ShipDesign>& design, const bool _delete)
{
    messageTypes::MsgShipDesign request(design, _delete);
    actions.shipDesignRequests.push_back(request);
}

void msg_queue::request_fleet_jump(unsigned int fleet_id)
{
    actions.fleetActionRequests.emplace_back(fleet_id, Fleet::Action::WarpLoading);
}

void msg_queue::request_fleet_build(unsigned int fleet_id)
{
    actions.fleetActionRequests.emplace_back(fleet_id, Fleet::Action::BuildAsteroidMine);
}

void msg_queue::request_fleet_colonize(unsigned int fleet_id)
{
    actions.fleetActionRequests.emplace_back(fleet_id, Fleet::Action::Colonize);
}

void msg_queue::request_fleet_civil_on(unsigned int fleet_id)
{
    actions.fleetActionRequests.emplace_back(fleet_id, Fleet::Action::KidnapCivilians);
}

void msg_queue::request_fleet_civil_off(unsigned int fleet_id)
{
    actions.fleetActionRequests.emplace_back(fleet_id, Fleet::Action::KickOutCivilians);
}

void msg_queue::request_fleet_soldiers_on(unsigned int fleet_id)
{
    actions.fleetActionRequests.emplace_back(fleet_id, Fleet::Action::BorrowSoldiers);
}

void msg_queue::request_fleet_soldiers_off(unsigned int fleet_id)
{
    actions.fleetActionRequests.emplace_back(fleet_id, Fleet::Action::DisembarkSoldiers);
}

void msg_queue::request_fleet_invade(unsigned int fleet_id)
{
    actions.fleetActionRequests.emplace_back(fleet_id, Fleet::Action::Invade);
}

void msg_queue::request_fleet_cancel(unsigned int fleet_id)
{
    actions.fleetActionRequests.emplace_back(fleet_id, Fleet::Action::None);
}
