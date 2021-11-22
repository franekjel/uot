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

void PlayersList::CountNumbers() {}
