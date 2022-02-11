#include "sector.h"
#include <cfloat>
#include "ship.h"

void Sector::JumpFleet(unsigned int fleet_id)
{
    if (present_fleets.count(fleet_id) < 1)
        return;
    float min_dist = FLT_MAX;
    std::shared_ptr<Sector> dest;
    auto this_fleet = present_fleets[fleet_id];
    for (const auto& neighbor : neighbors)
    {
        auto wersor_to_dest = (neighbor->position - position).normalized();
        auto dist = (this_fleet->position - wersor_to_dest).squaredLength();
        if (dist < min_dist)
        {
            min_dist = dist;
            dest = neighbor;
        }
    }

    this_fleet->JumpFleet();
    this_fleet->position = (position - dest->position).normalized();
    this_fleet->wanted_position = this_fleet->position;

    present_fleets.erase(present_fleets.find(fleet_id));

    DecrementWatcher(this_fleet->owner_id);

    dest->present_fleets[fleet_id] = this_fleet;
    dest->IncrementWatcher(this_fleet->owner_id);

    this_fleet->location_sector = dest;

    dest->jumped_fleets.push_back(
        Sector::JumpedFleet{fleet_id, sector_id, dest->sector_id, this_fleet->position, this_fleet->owner_id});
}

void Sector::DecrementWatcher(unsigned int player_id)
{
    if (watchers[player_id] == 0)
        throw;

    watchers[player_id]--;
}

void Sector::IncrementWatcher(unsigned int player_id)
{
    if (watchers.count(player_id) < 1)
        watchers[player_id] = 0;
    watchers[player_id] += 1;

    if (watchers[player_id] == 1)
        new_watchers[player_id] = 1;
}

Sector::FleetParameters::FleetParameters(std::shared_ptr<Fleet> fleet)
{
    fleet_id = fleet->id;
    position = fleet->position;
    soldiers = fleet->soldiers;
    civilians = fleet->civilians;
    human_capacity = fleet->human_capacity;
    construction_points = fleet->construction_points;
    current_hp = 0.0f;
    max_hp = 0.0f;
    current_shields = 0.0f;
    max_shields = 0.0f;
    average_energy = 0.0f;

    float tot_energy = 0.0f;

    if (fleet->ships.size() == 0)
    {
        base_fleet_speed = -1.0f;
    }
    else
    {
        base_fleet_speed = fleet->ships[0]->speed;
        for (const auto& ship : fleet->ships)
        {
            if (ship->speed < base_fleet_speed)
            {
                base_fleet_speed = ship->speed;
            }
            current_hp += ship->hp;
            max_hp += ship->max_hp;
            current_shields += ship->shield;
            max_shields += ship->max_shield;
            tot_energy += ship->energy / ship->max_energy;
        }
        average_energy = tot_energy / fleet->ships.size();
    }
}
