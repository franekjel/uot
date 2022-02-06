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
    watchers[this_fleet->owner_id]--;

    dest->present_fleets[fleet_id] = this_fleet;
    if (dest->watchers.count(this_fleet->owner_id) < 1)
        dest->watchers[this_fleet->owner_id] = 0;
    dest->watchers[this_fleet->owner_id] += 1;

    this_fleet->location_sector = dest;
}