#include "planet.h"
#include "player.h"

float Colony::population_building_modificator = 1.0f;

std::map<Building::BuildingType, int> Planet::GetAvailableBuildings(std::shared_ptr<Player> player)
{
    std::map<Building::BuildingType, int> res = {};

    if (!!colony && colony->owner->id != player->id)
        return res;

    for (const auto& b : possible_buildings)
    {
        const auto& available = player->available_buildings;
        if (find(available.begin(), available.end(), b.first) != available.end())
        {
            res[b.first] = b.second;
        }
    }

    if (!!colony)
    {
        for (const auto& b : colony->buildings)  // korekta o wybudowane budynki
        {
            if (res[b.first] != 0)
                res[b.first] -= b.second;
        }

        for (const auto& b : colony->building_queue)  // korekta o budynki w kolejce
        {
            if (res[b.type] != 0)
                res[b.type]--;
        }
    }

    return res;
}
