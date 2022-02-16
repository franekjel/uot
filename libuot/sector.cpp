#include "sector.h"
#include <cfloat>
#include "ship.h"

void Sector::JumpFleet(unsigned int fleet_id)
{
    if (present_fleets.count(fleet_id) < 1)
        return;
    float min_dist = FLT_MAX;
    std::shared_ptr<Sector> dest;
    std::shared_ptr<WarpZone> zone;
    auto this_fleet = present_fleets[fleet_id];
    for (const auto& warp_zone : warp_zones)
    {
        if (warp_zone->IsActive())
        {
            auto dist = (this_fleet->position - warp_zone->position).squaredLength();
            if (dist < min_dist)
            {
                min_dist = dist;
                dest = warp_zone->locationSector;
                zone = warp_zone;
            }
        }
    }

    this_fleet->JumpFleet();
    this_fleet->position = zone->connectedWarpZone->position;
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
        base_fleet_speed = fleet->ships.begin()->second->speed;
        for (const auto& [id, ship] : fleet->ships)
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

WarpZone::WarpZone(unsigned int id_, Point position_) : id(id_), position(position_) {}

bool WarpZone::IsActive() { return !!connectedWarpZone; }

void WarpZone::CreateWarpZonesNeighbours(std::shared_ptr<Sector> s1, std::shared_ptr<Sector> s2)
{
    auto wz1 = std::make_shared<WarpZone>(id_source++, (s2->position - s1->position).normalized());
    wz1->locationSector = s1;

    auto wz2 = std::make_shared<WarpZone>(id_source++, (s1->position - s2->position).normalized());
    wz2->locationSector = s2;

    wz1->connectedWarpZone = wz2;
    wz2->connectedWarpZone = wz1;

    s1->warp_zones.push_back(wz1);
    s2->warp_zones.push_back(wz2);
}
