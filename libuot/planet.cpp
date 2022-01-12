#include "planet.h"
#include "player.h"

float Colony::population_building_modificator = 1.0f;

std::map<Building::BuildingType, int> Colony::GetAvailableBuildings()
{
    std::map<Building::BuildingType, int> res = {};

    for (const auto& b : planet->possible_buildings)
    {
        const auto& available = owner->available_buildings;
        if (find(available.begin(), available.end(), b.first) != available.end())
        {
            res[b.first] = b.second;
        }
    }

    for (const auto& b : buildings)  // korekta o wybudowane budynki
    {
        if (res[b.first] != 0)
            res[b.first] -= b.second;
    }

    for (const auto& b : building_queue)  // korekta o budynki w kolejce
    {
        if (res[b.type] != 0)
            res[b.type]--;
    }

    for (const auto& b : UnlimitedBuildings)
    {
        const auto& available = owner->available_buildings;
        if (find(available.begin(), available.end(), b) != available.end())
        {
            res[b] = 999999;
        }
    }

    return res;
}
