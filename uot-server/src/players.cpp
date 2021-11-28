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
    for (const auto& sector : startingGalaxy->sectors)
    {
        for (const auto& sector_object : sector->objects)
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
    unsigned int id = player_id++;
    std::shared_ptr<Galaxy> startingGalaxy = GetStartingGalaxy(wholeGalaxy);
    std::shared_ptr<Colony> startingColony = GetStartingColony(id, startingGalaxy);
    std::shared_ptr<Player> new_player =
        std::make_shared<Player>(id, startingGalaxy, GetStartingResources(), startingColony);
    players[id] = new_player;
    players_net_names[id] = player_net_name;
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

void PlayersList::SendNewTourMessage(int tour_number, net_server_uot& messaging_service)
{
    for (auto& player : players)
    {
        messaging_service.send_new_tour_message(tour_number, player.second, players_net_names[player.first]);
    }
}

void PlayersList::CountWeeklyNumbersPlayer(std::shared_ptr<Player> player)
{
    auto& player_resources = player->owned_resources;
    auto& player_resources_change = player->resources_changed;
    auto& player_colonies = player->owned_colonies;
    auto& player_space_bases = player->owned_space_bases;
    auto& player_galaxy = player->known_galaxy;
    auto& player_ships = player->owned_ships;

    // calculate expenses and gains of player colonies
    for (auto& colony : player_colonies)
    {
        std::map<Resource, float> colony_gains = {};
        std::map<Resource, float> colony_expenses = {};
        int neccessary_workers = 0;

        for (const auto& buildingType : colony->buildings)
        {
            auto& building = Colony::GetBuildingFromType(buildingType.first);
            int number_of_buildings = buildingType.second;
            neccessary_workers += number_of_buildings * building.workers;
            for (auto& gains : building.production)
            {
                if (colony_gains.count(gains.first) == 0)
                    colony_gains[gains.first] = gains.second * number_of_buildings;
                else
                    colony_gains[gains.first] += gains.second * number_of_buildings;
            }

            for (const auto& expense : building.upkeep)
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

        for (auto& gain : colony_gains)
        {
            player_resources[gain.first] += gain.second;
            player_resources_change[gain.first] = true;
        }

        for (auto& expense : colony_expenses)
        {
            player_resources[expense.first] -= expense.second;
            player_resources_change[expense.first] = true;
        }
        
        player_resources[Resource::Food] -= colony->population * population_food_usage;
        player_resources_change[Resource::Food] = true;

        if (player_resources[Resource::Food] >= 0)
        {
            colony->population += colony->population * colony->base_population_growth;
            colony->population_changed = true;
        }
        else
        {
            colony->population -= colony->population * colony->base_population_growth;
            colony->population_changed = true;
        }

        // what if there are less resources available than there are needed to operate colony?
        // currently I assume that negative number is possible (like public debt XD)
    }

    // calculate bilans of inhabitable objects

    for (auto& space_base : player_space_bases)
    {
        for (auto& resource : space_base->object->resurce_deposit)
        {
            player_resources[resource.first] += resource.second;
            player_resources_change[resource.first] = true;
        }

        // If space base get some upkeep cost, calculate it here;
    }
}

void PlayersList::CountEveryTourNumbersPlayer(std::shared_ptr<Player> player) {}

void PlayersList::CountEveryTourNumbers()
{
    // Async Part like ships upkeep costs
    std::vector<std::thread> player_threads(players.size());
    int player_num = 0;
    for (auto& player : players)
        player_threads[player_num++] = std::thread(CountEveryTourNumbersPlayer, player.second);

    for (auto& thread : player_threads)
        thread.join();
}
