#include "../headers/players.h"

unsigned int PlayersList::GetStartingSectorId(std::shared_ptr<Galaxy> wholeGalaxy)
{
    for (const auto& [id, sector] : wholeGalaxy->sectors)
    {
        int num_of_planets = 0;
        for (const auto& [id, sector_object] : sector->objects)
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
            return id;
        }
    }

    return UINT_MAX;
}

std::map<Resource, float> PlayersList::GetStartingResources()
{
    // TODO: Prepare set of starting resources
    return {{Resource::Food, 100.0f}, {Resource::Metals, 100.0f}};
}

unsigned int PlayersList::GetStartingColonyObjId(unsigned int player_id, unsigned int startingSectorId,
                                                 std::shared_ptr<Galaxy> wholeGalaxy)
{
    std::shared_ptr<Planet> planet = nullptr;
    auto& sector = wholeGalaxy->sectors[startingSectorId];
    for (const auto& [id, sector_object] : sector->objects)
    {
        std::shared_ptr<Planet> is_planet = std::dynamic_pointer_cast<Planet>(sector_object);
        if (!!is_planet && !(is_planet->colony))
        {
            planet = is_planet;
            break;
        }
    }
    if (!planet)
        return UINT_MAX;
    std::shared_ptr<Colony> startingColony = std::make_shared<Colony>(id_source++, planet);
    planet->colony = startingColony;
    sector->watchers.insert({player_id, 1});
    return planet->id;
}

bool PlayersList::AddPlayer(std::string player_net_name, std::shared_ptr<Galaxy> wholeGalaxy)
{
    unsigned int id = player_id++;
    unsigned int startingSectorId = GetStartingSectorId(wholeGalaxy);
    if (startingSectorId == UINT_MAX)
        return false;
    unsigned int startingColonyObjId = GetStartingColonyObjId(id, startingSectorId, wholeGalaxy);
    if (startingColonyObjId == UINT_MAX)
        return false;
    std::shared_ptr<Player> new_player =
        std::make_shared<Player>(id, wholeGalaxy, GetStartingResources(), startingSectorId, startingColonyObjId);
    if (!new_player)
        return false;
    auto& obj = wholeGalaxy->sectors[startingSectorId]->objects[startingColonyObjId];
    std::shared_ptr<Planet> startingPlanet = std::dynamic_pointer_cast<Planet>(obj);
    startingPlanet->colony->owner = new_player;
    players[id] = new_player;
    players_net_names[id] = player_net_name;
    players_net_names_rev[player_net_name] = id;

    return true;
}

bool PlayersList::HandlePlayerRequests(std::string player_net_name,
                                       std::shared_ptr<messageTypes::ActionsPayload> payload)
{
    auto player = players[players_net_names_rev[player_net_name]];
    for (const auto& build : payload->buildRequests)
    {
        player->HandleBuildRequest(build.building_type, build.upgrade_from, build.colony_id);
    }

    for (const auto& move_request : payload->moveFleetRequests)
    {
        player->HandleMoveFleetRequest(move_request.fleet_id, move_request.position);
    }

    if (payload->technologyRequest != Technology::TechnologyType::Empty)
        player->HandleStartTechnologyResearch(payload->technologyRequest);

    for (const auto& join_fleet_request : payload->joinFleetsRequests)
    {
        player->HandleJoinFleetRequest(join_fleet_request.fleet1_id, join_fleet_request.fleet2_id);
    }

    for (const auto& fleet_action_request : payload->fleetActionRequests)
    {
        switch (fleet_action_request.action)
        {
            case Fleet::Action::BuildAsteroidMine:
                player->HandleBuildAsteroidMineFleetRequest(fleet_action_request.fleet_id);
            case Fleet::Action::CancelAction:
                player->HandleCancelFleetRequest(fleet_action_request.fleet_id);
            case Fleet::Action::Colonize:
                player->HandleColonizeFleetRequest(fleet_action_request.fleet_id);
            case Fleet::Action::Invade:
            case Fleet::Action::None:
                break;

            case Fleet::Action::WarpLoading:
                player->HandleWarpLoadingFleetRequest(fleet_action_request.fleet_id);
                break;

            default:
                break;
        }
    }

    return true;
}

void PlayersList::CountWeeklyNumbers()
{
    // std::vector<std::thread> player_threads(players.size());
    // int player_num = 0;
    // for (auto& player : players)
    //    player_threads[player_num++] = std::thread(CountWeeklyNumbersPlayer, player.second);

    // for (auto& thread : player_threads)
    //    thread.join();

    // Let it be synchornic for now - can be done async but too much work for now

    for (auto& player : players)
        CountWeeklyNumbersPlayer(player.second);

    // here are weekly actions performed that could not have been done multithreadedly
    for (auto& [player_id, player] : players)
    {
        auto& player_fleets = player->owned_fleets;
        for (const auto& [fleet_id, fleet] : player_fleets)
        {
            if (fleet->current_action == Fleet::Action::BuildAsteroidMine)
            {
                if (fleet->building_progress >= fleet->full_building_progress)
                {
                    if (fleet->base_building_object->base)
                    {
                        fleet->base_building_object->base->owner->owned_space_bases.erase(
                            fleet->base_building_object->base->owner->owned_space_bases.find(
                                fleet->base_building_object->base->id));
                        player->known_galaxy->sectors[fleet->base_building_object->sector_id]->DecrementWatcher(
                            fleet->base_building_object->base->owner->id);
                    }
                    auto& bb_object = fleet->base_building_object;
                    bb_object->base = std::make_shared<SpaceBase>(id_source++, bb_object, player);
                    auto& base = bb_object->base;
                    fleet->current_action = Fleet::Action::None;
                    player->owned_space_bases[base->id] = base;
                    player->known_galaxy->sectors[bb_object->sector_id]->IncrementWatcher(base->owner->id);

                    fleet->building_progress = 0.0f;
                    fleet->full_building_progress = 0.0f;
                    fleet->base_building_object = nullptr;

                    auto& sector = player->known_galaxy->sectors[fleet->base_building_object->sector_id];
                    sector->new_bases.push_back({base->id, bb_object->id, player->id});
                }
            }
            else if (fleet->current_action == Fleet::Action::Colonize)
            {
                if (fleet->building_progress >= fleet->full_building_progress)
                {
                    auto& cb_object = fleet->colony_building_object;
                    cb_object->colony = std::make_shared<Colony>(id_source++, cb_object);
                    auto& colony = cb_object->colony;
                    player->owned_colonies[fleet->colony_building_object->colony->id] =
                        fleet->colony_building_object->colony;
                    fleet->colony_building_object->colony->population = Fleet::kColonizationCost;
                    player->known_galaxy->sectors[fleet->colony_building_object->sector_id]->IncrementWatcher(
                        fleet->colony_building_object->colony->owner->id);
                    fleet->current_action = Fleet::Action::None;
                    fleet->building_progress = 0.0f;
                    fleet->full_building_progress = 0.0f;
                    fleet->colony_building_object = nullptr;

                    auto& sector = player->known_galaxy->sectors[fleet->base_building_object->sector_id];
                    sector->new_colonies.push_back({colony->id, cb_object->id, player->id, colony->population});
                }
            }
        }
    }
}

void PlayersList::SendNewTurnMessage(int turn_number, net_server_uot& messaging_service,
                                     std::shared_ptr<Galaxy>& galaxy)
{
    for (auto& player : players)
    {
        messaging_service.send_new_turn_message(turn_number, player.second, players_net_names[player.first], galaxy);
    }

    // cleanup

    for (auto& [id, sector] : galaxy->sectors)
    {
        sector->joined_fleets.clear();
        sector->new_watchers.clear();
        sector->new_bases.clear();
        sector->new_colonies.clear();
    }
}

void PlayersList::SendStartGameMessage(net_server_uot& messaging_service, std::shared_ptr<Galaxy>& galaxy)
{
    for (auto& player : players)
    {
        messaging_service.send_game_begin_message(player.second, players_net_names[player.first], galaxy);
    }
}

void PlayersList::CountWeeklyNumbersPlayer(std::shared_ptr<Player> player)
{
    auto& player_resources = player->owned_resources;
    auto& player_resources_change = player->resources_changed;
    auto& player_colonies = player->owned_colonies;
    auto& player_space_bases = player->owned_space_bases;
    auto& player_fleets = player->owned_fleets;
    auto& player_research = player->researched_technology;

    // technology points are special, as they disappear if not used
    player_resources[Resource::Technology] = 0.0f;
    player_resources_change[Resource::Technology] = true;
    player_resources[Resource::Food] = 0.0f;
    player_resources_change[Resource::Food] = true;

    // This is a temporary debuggingsolution please remove ASAP
    for (int i = (int)Resource::Metals; i <= (int)Resource::Last; i++)
    {
        if (player_resources.count((Resource)i) == 0)
            player_resources[(Resource)i] = 0.0f;
        player_resources[(Resource)i] += 1.0f;
        player_resources_change[(Resource)i] = true;
    }

    // calculate expenses and gains of player colonies
    for (auto& colony : player_colonies)
    {
        std::map<Resource, float> colony_gains = {};
        std::map<Resource, float> colony_expenses = {};

        colony_gains = colony.second->GetColonyGains();
        colony_expenses = colony.second->GetColonyExpenses();

        for (auto& gain : colony_gains)
        {
            if (gain.first == Resource::Food)
                continue;
            if (player_resources.count(gain.first) == 0)
                player_resources[gain.first] = 0.0f;
            player_resources[gain.first] += gain.second;
            player_resources_change[gain.first] = true;
        }

        for (auto& expense : colony_expenses)
        {
            if (expense.first == Resource::Food)
                continue;
            if (player_resources.count(expense.first) == 0)
                player_resources[expense.first] = 0.0f;
            player_resources[expense.first] -= expense.second;
            player_resources_change[expense.first] = true;
        }

        float food_bilans = colony_gains[Resource::Food] - colony.second->population * population_food_usage;

        if (food_bilans >= 0)
        {
            colony.second->population +=
                colony.second->population * colony.second->base_population_growth * player->population_growth_modifier;
            colony.second->population_changed = true;
        }
        else
        {
            colony.second->population -= colony.second->population * colony.second->base_population_starving_death;
            if (colony.second->population < base_population)
                colony.second->population = base_population;
            colony.second->population_changed = true;
        }

        // what if there are less resources available than there are needed to operate colony?
        // currently I assume that negative number is possible (like public debt XD)

        // update building queue
        colony.second->UpdateBuildingQueue();
    }

    // calculate bilans of inhabitable objects

    for (auto& space_base : player_space_bases)
    {
        for (auto& resource : space_base.second->object->resource_deposit)
        {
            if (player_resources.count(resource.first) == 0)
                player_resources[resource.first] = 0.0f;
            player_resources[resource.first] +=
                resource.second * player->resources_modifiers_inhabitable[resource.first];
            player_resources_change[resource.first] = true;
        }

        // If space base get some upkeep cost, calculate it here;
    }

    // use technology points to research technology if any is being researched
    if (player_research)
    {
        if (player_resources[Resource::Technology] >= player_research.progress_left)
        {
            player_research.progress_left = 0.0f;
            player->DiscoverTechnology(player_research.technology);
            player_research = {};
        }
        else
            player_research.progress_left -= player_resources[Resource::Technology];
    }

    for (const auto& [fleet_id, fleet] : player_fleets)
    {
        if (fleet->current_action == Fleet::Action::BuildAsteroidMine ||
            fleet->current_action == Fleet::Action::Colonize)
        {
            fleet->building_progress += fleet->construction_points;
        }
    }
}

void PlayersList::CountEveryTurnNumbersPlayer(std::shared_ptr<Player> player)
{
    for (const auto& fleet : player->owned_fleets)
    {
        fleet.second->UpdateFleet();
    }
}

void PlayersList::CountEveryTurnNumbers()
{
    //// Async Part like ships upkeep costs
    // std::vector<std::thread> player_threads(players.size());
    // int player_num = 0;
    // for (auto& player : players)
    //    player_threads[player_num++] = std::thread(CountEveryTurnNumbersPlayer, player.second);

    // for (auto& thread : player_threads)
    //    thread.join();

    for (auto& player : players)
        CountEveryTurnNumbersPlayer(player.second);
}
