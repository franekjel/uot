#include "../headers/players.h"

std::shared_ptr<Galaxy> PlayersList::GetStartingGalaxy(std::shared_ptr<Galaxy> wholeGalaxy)
{
    // TODO: Copy one empty (without another player) sector with at least one planet from wholeGalaxy
    auto known_galaxy = std::make_shared<Galaxy>();
    std::shared_ptr<Sector> starting_sector;
    for (const auto& sector : wholeGalaxy->sectors)
    {
        int num_of_planets = 0;
        for (const auto& sector_object : sector->objects)
        {
            std::shared_ptr<Planet> is_planet = std::dynamic_pointer_cast<Planet>(sector_object);
            if (!!is_planet)
            {
                num_of_planets++;
                if (!!(is_planet->colony))
                {
                    num_of_planets = -1;
                    break;
                }
            }
        }
        if (num_of_planets > 0)
        {
            std::shared_ptr<Sector> new_sector = std::make_shared<Sector>();

            new_sector->sector_id = sector->sector_id;
            new_sector->position = sector->position;

            for (auto& neigh : sector->neighbors)
            {
                auto new_neigh = std::make_shared<Sector>();
                new_neigh->sector_id = neigh->sector_id;
                new_neigh->position = neigh->position;
                new_sector->neighbors.insert(new_neigh);
                known_galaxy->sectors.insert(new_neigh);
            }

            for (auto& obj : sector->objects)
            {
                auto& star = std::dynamic_pointer_cast<Star>(obj);
                auto& inhabitable = std::dynamic_pointer_cast<InhabitableObject>(obj);
                auto& planet = std::dynamic_pointer_cast<Planet>(obj);

                if (!!star)
                {
                    new_sector->objects.insert(
                        std::make_shared<Star>(SectorObject(star->id, star->position, star->size), star->star_type));
                }
                else if (!!inhabitable)
                {
                    new_sector->objects.insert(std::make_shared<InhabitableObject>(
                        SectorObject(inhabitable->id, inhabitable->position, inhabitable->size),
                        std::map<Resource, float>(), inhabitable->object_type));
                }
                else if (!!planet)
                {
                    auto new_planetary_features = std::set<PlanetaryFeatures::PlanetaryFeatureType>();
                    for (auto feat : planet->planetary_features)
                    {
                        new_planetary_features.insert(feat);
                    }
                    new_sector->objects.insert(
                        std::make_shared<Planet>(SectorObject(planet->id, planet->position, planet->size),
                                                 planet->climate, new_planetary_features));
                }
            }

            known_galaxy->sectors.insert(new_sector);
            break;
        }
    }

    return known_galaxy;
}

std::map<Resource, float> PlayersList::GetStartingResources()
{
    // TODO: Prepare set of starting resources
    return {};
}

std::shared_ptr<Colony> PlayersList::GetStartingColony(long player_id, std::shared_ptr<Galaxy> startingGalaxy,
                                                       std::shared_ptr<Galaxy> wholeGalaxy)
{
    std::shared_ptr<Planet> planet = nullptr;
    int starting_sector_id = -1;
    for (const auto& sector : startingGalaxy->sectors)
    {
        for (const auto& sector_object : sector->objects)
        {
            std::shared_ptr<Planet> is_planet = std::dynamic_pointer_cast<Planet>(sector_object);
            if (!!is_planet && !(is_planet->colony))
            {
                planet = is_planet;
                break;
            }
        }
        if (!!planet)
            break;
        starting_sector_id = sector->sector_id;
    }
    if (!planet)
        throw std::runtime_error("No empty planet found");
    std::shared_ptr<Colony> startingColony = std::make_shared<Colony>(player_id, planet);
    planet->colony = startingColony;
    for (auto& sector : wholeGalaxy->sectors)
    {
        if (sector->sector_id != starting_sector_id)
            continue;
        for (auto& pl : sector->objects)
        {
            if (pl->id != planet->id)
                continue;
            auto& plan = std::dynamic_pointer_cast<Planet>(pl);
            plan->colony = startingColony;
        }
    }
    return startingColony;
}

void PlayersList::AddPlayer(std::string player_net_name, std::shared_ptr<Galaxy> wholeGalaxy)
{
    unsigned int id = player_id++;
    std::shared_ptr<Galaxy> startingGalaxy = GetStartingGalaxy(wholeGalaxy);
    std::shared_ptr<Colony> startingColony = GetStartingColony(id, startingGalaxy, wholeGalaxy);

    std::shared_ptr<Player> new_player =
        std::make_shared<Player>(id, startingGalaxy, GetStartingResources(), startingColony);
    startingColony->owner = new_player;
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

void PlayersList::SendStartGameMessage(net_server_uot& messaging_service)
{
    for (auto& player : players)
    {
        messaging_service.send_game_begin_message(player.second, players_net_names[player.first]);
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

        colony_gains = colony->GetColonyGains();
        colony_expenses = colony->GetColonyExpenses();

        for (auto& gain : colony_gains)
        {
            if (gain.first == Resource::Food)
                continue;
            player_resources[gain.first] += gain.second;
            player_resources_change[gain.first] = true;
        }

        for (auto& expense : colony_expenses)
        {
            if (expense.first == Resource::Food)
                continue;
            player_resources[expense.first] -= expense.second;
            player_resources_change[expense.first] = true;
        }

        float food_bilans = colony_gains[Resource::Food] - colony->population * population_food_usage;

        if (food_bilans >= 0)
        {
            colony->population += colony->population * colony->base_population_growth;
            colony->population_changed = true;
        }
        else
        {
            colony->population -= colony->population * colony->base_population_starving_death;
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
