#include "net_server_uot.h"
#include <player.h>
#include <iostream>

#define INF_TIME 999999
#define EPS 0.0001f

net_server_uot::net_server_uot() : txrx(*this, 40), running(false) { txrx.set_as_handler(); }

bool net_server_uot::accept_player(const server_txrx::net_player& player)
{
    std::cout << "Player " << player.name << " wants to connect with address " << player.IP << "\n";
    auto res = uot_accept_player(player.name);
    if (res)
        players.push_back(player.name);
    return res;
}

void net_server_uot::after_accept_player() { uot_after_accept_player(); }

void net_server_uot::handle_status_change(const std::string& player_name, net_server::net_status status)
{
    if (status == net_server::net_status::disconnect)
    {
        std::cout << "Player " << player_name << " disconnected\n";
        for (int i = 0; i < players.size(); i++)
        {
            if (players[i] == player_name)
            {
                players.erase(players.begin() + i);
                break;
            }
        }
    }
}
void net_server_uot::handle_message(const std::string& player_name, const std::string& data)
{
    std::cout << player_name << ": " << data << "\n";

    auto deserialized = messageTypes::Deserialize(data);
    if (!!deserialized)
    {
        if (deserialized->GetType() == messageTypes::MessageType::Actions)
        {
            auto cast = std::dynamic_pointer_cast<messageTypes::ActionsPayload>(deserialized);
            uot_handle_actions(player_name, cast);
        }
    }
}

void net_server_uot::run()
{
    uint16 port_num = 7645;
    txrx.start_socket(port_num);
    std::cout << "Server listening on port " << port_num;
    running = true;
}

void net_server_uot::set_accept_player_callback(std::function<bool(std::string player_name)> callback)
{
    uot_accept_player = callback;
}

void net_server_uot::set_handle_actions_callback(
    std::function<void(std::string, std::shared_ptr<messageTypes::ActionsPayload>)> callback)
{
    uot_handle_actions = callback;
}

void net_server_uot::set_after_accept_player_callback(std::function<void()> callback)
{
    uot_after_accept_player = callback;
}

void net_server_uot::send_new_turn_message(int turn_number, std::shared_ptr<Player>& player,
                                           std::string player_net_name, std::shared_ptr<Galaxy>& galaxy)
{
    messageTypes::NewTurnPayload payload;

    for (const auto& resource : player->owned_resources)
    {
        if (player->resources_changed[resource.first])
        {
            payload.updated_resources[resource.first] = resource.second;
            player->resources_changed[resource.first] = false;
        }
    }

    for (auto& colony : player->owned_colonies)
    {
        if (colony.second->population_changed)
        {
            payload.updated_populations[colony.second->id] = colony.second->population;
            colony.second->population_changed = false;
        }

        if (colony.second->building_queue_changed)
        {
            float work_offset = 0.0f;
            float divider = colony.second->population_building_modificator * colony.second->unemployed_population;

            for (const auto& building : colony.second->building_queue)
            {
                if (divider > EPS)
                    work_offset += building.worker_week_units_left / divider;
                else
                    work_offset = INF_TIME;

                int days_remaining = work_offset;
                if (days_remaining == 0)
                    days_remaining++;

                payload.buildings_updates.push_back(messageTypes::MsgBuildingsUpdates(
                    colony.second->id, building.type, building.upgrade_of, days_remaining));
            }
            colony.second->building_queue_changed = false;
        }

        if (colony.second->ship_building_queue_changed)
        {
            float people_weeks_to_distribute = 0.0f;
            for (const auto& [buld_type, count] : colony.second->buildings)
            {
                if (buld_type == Building::BuildingType::SmallOrbitalShipyard ||
                    buld_type == Building::BuildingType::MediumOrbitalShipyard ||
                    buld_type == Building::BuildingType::GrandOrbitalShipyard)
                {
                    people_weeks_to_distribute += Colony::GetBuildingFromType(buld_type).workers *
                                                  colony.second->buildings_working_modifier * count;
                }
            }

            float work_offset = 0.0f;
            float divider = people_weeks_to_distribute;

            for (const auto& ship : colony.second->ship_building_queue)
            {
                if (divider > EPS)
                    work_offset += ship.worker_week_units_left / divider;
                else
                    work_offset = INF_TIME;

                int days_remaining = work_offset;
                if (days_remaining == 0)
                    days_remaining++;

                payload.ships_updates.push_back(
                    messageTypes::MsgShipUpdate(ship.design->id, colony.second->planet->id, days_remaining));
            }
            colony.second->ship_building_queue_changed = false;
        }

        for (const auto& new_building : colony.second->new_buildings)
        {
            payload.buildings_updates.push_back(
                messageTypes::MsgBuildingsUpdates(colony.second->id, new_building.type, new_building.upgrade_of, 0));
        }
        for (const auto& new_ship : colony.second->new_ships)
        {
            payload.new_ships.push_back(messageTypes::MsgNewShip(new_ship.design->id, colony.second->planet->id,
                                                                 new_ship.new_fleet, new_ship.ship->id,
                                                                 Sector::FleetParameters(new_ship.ship->fleet)));
        }

        colony.second->new_buildings.clear();
        colony.second->new_ships.clear();
    }

    for (const auto& [sector_id, sector] : galaxy->sectors)
    {
        if (sector->new_watchers.count(player->id))
        {
            payload.new_sectors.push_back(messageTypes::MsgSector(sector));
        }
        if (sector->watchers.count(player->id) != 0 && sector->watchers[player->id] > 0)
        {
            auto sector_update_msg = messageTypes::MsgWatchedSectorUpdate(sector_id);
            for (const auto& [id, fleet] : sector->present_fleets)
            {
                sector_update_msg.fleets.push_back(messageTypes::MsgFleet(fleet, fleet->owner_id));
            }

            for (const auto& new_base : sector->new_bases)
            {
                sector_update_msg.new_bases.push_back(messageTypes::MsgNewBase(new_base));
            }

            for (const auto& new_colony : sector->new_colonies)
            {
                sector_update_msg.new_colonies.push_back(messageTypes::MsgNewColony(new_colony));
            }

            for (const auto& joined_fleet : sector->joined_fleets)
            {
                if (joined_fleet.owner == player->id)
                    payload.joined_fleets.push_back(messageTypes::MsgFleetsJoin(joined_fleet));
            }

            for (const auto& jumped_fleet : sector->jumped_fleets)
            {
                if (jumped_fleet.owner == player->id)
                    payload.jumped_fleets.push_back(messageTypes::MsgFleetsJump(jumped_fleet));
            }

            payload.watched_sectors_updates.push_back(sector_update_msg);
        }
    }

    if (player->researched_technology.technology != Technology::TechnologyType::None)
    {
        float divider = player->owned_resources[Resource::Technology];
        float tech_offset = 0.0f;

        if (divider > EPS)
            tech_offset += player->researched_technology.progress_left / divider;
        else
            tech_offset = INF_TIME;

        int days_remaining = tech_offset;
        if (days_remaining == 0)
            days_remaining++;

        auto technology_update_msg =
            messageTypes::MsgTechnologyUpdate(player->researched_technology.technology, days_remaining);
        payload.technology_updates.push_back(technology_update_msg);
    }

    for (const auto& new_technology : player->new_technologies)
    {
        auto technology_update_msg = messageTypes::MsgTechnologyUpdate(new_technology, 0);
        payload.technology_updates.push_back(technology_update_msg);
    }

    for (const auto& changed_design : player->changed_designs)
    {
        auto ship_design = messageTypes::MsgShipDesignResponse(changed_design.design_id, changed_design.design,
                                                               changed_design.deleted);
        payload.ship_designs.push_back(ship_design);
    }

    player->new_technologies.clear();
    player->changed_designs.clear();

    txrx.send_reliable(player_net_name, payload.Serialize());
}

void net_server_uot::send_game_begin_message(std::shared_ptr<Player>& player, std::string player_net_name,
                                             std::shared_ptr<Galaxy>& galaxy)
{
    messageTypes::StartGamePayload payload;

    payload.player_id = player->id;
    payload.galaxy = messageTypes::MsgGalaxy(galaxy, player);
    payload.starting_resources = {};
    for (const auto& resource : player->owned_resources)
    {
        payload.starting_resources[resource.first] = resource.second;
    }
    payload.starting_ships_designs.reserve(player->ship_designs.size());
    for (const auto& d : player->ship_designs)
    {
        payload.starting_ships_designs.emplace_back(messageTypes::MsgShipDesignResponse(d.second->id, d.second, false));
    }

    txrx.send_reliable(player_net_name, payload.Serialize());
}

void net_server_uot::read_input()
{
    while (running)
    {
        char command;
        std::cin >> command;
        if (command == 'e')
            running = false;
        else
            std::cout << "Unknown command " << std::string(&command, 1);
    }
}
