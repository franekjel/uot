#include "../headers/players.h"

std::shared_ptr<Galaxy> PlayersList::GetStartingGalaxy(std::shared_ptr<Galaxy> wholeGalaxy)
{
    // TODO: Copy one empty (without another player) sector with at least one planet from wholeGalaxy
    return wholeGalaxy;
    return std::make_shared<Galaxy>();
}

std::map<Resource, float> PlayersList::GetStartingResources()
{
    // TODO: Prepare set of starting resources
    return {};
}

std::shared_ptr<Colony> PlayersList::GetStartingColony(long player_id, std::shared_ptr<Galaxy> startingGalaxy)
{
    std::shared_ptr<Planet> planet = nullptr;
    for (auto& sector : startingGalaxy->sectors)
    {
        for (auto& sector_object : sector->objects)
        {
            std::shared_ptr<Planet> is_planet = std::dynamic_pointer_cast<Planet>(sector_object);
            if (!!is_planet)
            {
                planet = is_planet;
                break;
            }
        }
        if (!!planet)
            break;
    }
    if (!planet)
        throw std::runtime_error("No empty planet found");
    std::shared_ptr<Colony> startingColony = std::make_shared<Colony>(player_id, planet);
    return startingColony;
}

void PlayersList::AddPlayer(std::string player_net_name, std::shared_ptr<Galaxy> wholeGalaxy)
{
    long id = player_id++;
    std::shared_ptr<Galaxy> startingGalaxy = GetStartingGalaxy(wholeGalaxy);
    std::shared_ptr<Colony> startingColony = GetStartingColony(id, startingGalaxy);
    std::shared_ptr<Player> new_player =
        std::make_shared<Player>(id, player_net_name, startingGalaxy, GetStartingResources(), startingColony);
    players[id] = new_player;
}

void PlayersList::CountWeeklyNumbers()
{
    std::vector<std::thread> player_threads(players.size());
    int player_num = 0;
    for (auto& player : players)
        player_threads[player_num++] = std::thread(CountWeeklyNumbersPlayer, player.second);

    for (auto& thread : player_threads)
        thread.join();
}

void PlayersList::CountWeeklyNumbersPlayer(std::shared_ptr<Player> player)
{
    auto& player_resources = player->owned_resources;
    auto& player_colonies = player->owned_colonies;
    auto& player_galaxy = player->known_galaxy;
    auto& player_ships = player->owned_ships;

    // calculate expenses and gains of player colonies
    for (auto& colony : player_colonies)
    {
        std::map<Resource, float> colony_gains = {};
        std::map<Resource, float> colony_expenses = {};
        int neccessary_workers = 0;

        for (auto& building : colony->buildings)
        {
            int number_of_buildings = building.second;
            neccessary_workers += number_of_buildings * building.first.workers;
            for (auto& gains : building.first.production)
            {
                if (colony_gains.count(gains.first) == 0)
                    colony_gains[gains.first] = gains.second * number_of_buildings;
                else
                    colony_gains[gains.first] += gains.second * number_of_buildings;
            }

            for (auto& expense : building.first.upkeep)
            {
                if (colony_expenses.count(expense.first) == 0)
                    colony_expenses[expense.first] = expense.second * number_of_buildings;
                else
                    colony_expenses[expense.first] += expense.second * number_of_buildings;
            }
        }

        float colony_efficency =
            neccessary_workers > colony->population ? colony->population / neccessary_workers : 1.0f;

        if (colony_efficency < 1.0f)
        {
            colony_gains = colony_gains * colony_efficency;
            colony_expenses = colony_expenses * colony_efficency;
            // I assumed that if building is less efficient it is also cheaper to upkeep
        }

        player_resources += colony_gains;
        for (auto& expense : colony_expenses)
            player_resources[expense.first] -= expense.second;
        // what if there are less resources available than there are needed to operate colony?
        // currently I assume that negative number is possible (like public debt XD)
    }

    // calculate bilans of inhabitable objects currently mines has no upkeep cost - let's assume that they are run by
    // robots and operates on solar energy etc.
    for (auto& sector : player_galaxy->sectors)
    {
        for (auto& object : sector->objects)
        {
            auto inhabitable_object = std::dynamic_pointer_cast<InhabitableObject>(object);
            if (!inhabitable_object)
                continue;
            if (inhabitable_object->owner_id == player->player_id)
                for (auto& mine : inhabitable_object->resurce_mine_built)
                {
                    if (mine.second)
                    {
                        player_resources[mine.first] += inhabitable_object->resurce_deposit[mine.first];
                    }
                }
        }
    }
}

void PlayersList::CountEveryTourNumbers() {}
