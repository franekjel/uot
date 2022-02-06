#include "sector.h"
#include "ship.h"

void Sector::JumpFleet(unsigned int fleet_id)
{
    if (present_fleets.count(fleet_id) < 1)
        return;
    float min_dist = FLT_MAX;
    std::shared_ptr<Sector> dest;
    auto this_fleet = present_fleets[fleet_id];
    for (const auto &neighbor : neighbors)
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

    present_fleets.erase(present_fleets.find(fleet_id));

    DecrementWatcher(this_fleet->owner_id);

    dest->present_fleets[fleet_id] = this_fleet;
    dest->IncrementWatcher(this_fleet->owner_id);

    this_fleet->location_sector = dest;
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