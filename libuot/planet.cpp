#include "planet.h"
#include "player.h"

float Colony::population_building_modificator = 1.0f;

std::map<Resource, float> Colony::GetColonyGains()
{
    std::map<Resource, float> colony_gains = {};
    int neccessary_workers = 0;

    for (auto& buildingType : buildings)
    {
        auto& building = Colony::GetBuildingFromType(buildingType.first);
        int number_of_buildings = buildingType.second;
        neccessary_workers += number_of_buildings * building.workers;
        for (auto& gains : building.production)
        {
            if (colony_gains.count(gains.first) == 0)
                colony_gains[gains.first] =
                    gains.second * number_of_buildings * owner->resources_modifiers_planet[gains.first];
            else
                colony_gains[gains.first] +=
                    gains.second * number_of_buildings * owner->resources_modifiers_planet[gains.first];
        }
    }

    float colony_efficency = neccessary_workers > population ? population / neccessary_workers : 1.0f;
    unemployed_population = population - neccessary_workers;

    if (colony_efficency < 1.0f)
        colony_gains = colony_gains * colony_efficency;

    return colony_gains;
}

std::map<Resource, float> Colony::GetColonyExpenses()
{
    std::map<Resource, float> colony_expenses = {};
    int neccessary_workers = 0;

    for (auto& buildingType : buildings)
    {
        auto& building = Colony::GetBuildingFromType(buildingType.first);
        int number_of_buildings = buildingType.second;
        neccessary_workers += number_of_buildings * building.workers;

        for (auto& expense : building.upkeep)
        {
            if (colony_expenses.count(expense.first) == 0)
                colony_expenses[expense.first] = expense.second * number_of_buildings;
            else
                colony_expenses[expense.first] += expense.second * number_of_buildings;
        }
    }

    float colony_efficency = neccessary_workers > population ? population / neccessary_workers : 1.0f;

    if (colony_efficency < 1.0f)
        colony_expenses = colony_expenses * colony_efficency;
    // I assumed that if building is less efficient it is also cheaper to upkeep

    return colony_expenses;
}

void Colony::UpdateBuildingQueue()
{
    if (building_queue.size() == 0)
        return;

    float people_weeks_to_distribute = unemployed_population * population_building_modificator;
    auto itr = building_queue.begin();
    while (people_weeks_to_distribute > 0.0f && itr != building_queue.end())
    {
        if (itr->worker_week_units_left < people_weeks_to_distribute)
        {
            people_weeks_to_distribute -= itr->worker_week_units_left;
            itr->worker_week_units_left = 0.0f;
        }
        else
        {
            itr->worker_week_units_left -= people_weeks_to_distribute;
            people_weeks_to_distribute = 0.0f;
        }
        itr++;
    }

    while (building_queue.size() != 0 && building_queue.front().worker_week_units_left == 0.0f)
    {
        if (building_queue.front().upgrade_of != Building::BuildingType::None)
            buildings[building_queue.front().upgrade_of]--;
        buildings[building_queue.front().type]++;
        new_buildings.push_back(building_queue.front());
        building_queue.erase(building_queue.begin());
    }

    building_queue_changed = true;
    return;
}

void Colony::AddBuildingToQueue(Building::BuildingType type, Building::BuildingType upgrade_from)
{
    building_queue.push_back({type, upgrade_from});
    building_queue_changed = true;
}

void Colony::RemoveBuildingFromQueueOnPosition(unsigned int position)
{
    if (position < building_queue.size())
        building_queue.erase(building_queue.begin() + position);
    building_queue_changed = true;
}

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

Colony::Colony(const unsigned int id, const std::shared_ptr<Planet> planet_) : id(id)
{
    population = 1;
    buildings = {};
    planet = planet_;
    unemployed_population = population;
    building_queue = {};
    building_queue_changed = false;
}
