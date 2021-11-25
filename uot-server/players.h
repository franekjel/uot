#pragma once
#include <memory>
#include <stdexcept>
#include <vector>

#include "galaxy.h"
#include "player.h"

static long player_id = 1;
class PlayersList
{
   private:
    std::map<long, std::shared_ptr<Player>> players = {};

   public:
    std::shared_ptr<Galaxy> GetStartingGalaxy(std::shared_ptr<Galaxy> wholeGalaxy)
    {
        // TODO: Copy one empty (without another player) sector with at least one planet from wholeGalaxy
        return std::make_shared<Galaxy>();
    }

    std::map<Resource, float> GetStartingResources()
    {
        // TODO: Prepare set of starting resources
        return {};
    }

    std::shared_ptr<Colony> GetStartingColony(std::shared_ptr<Galaxy> startingGalaxy)
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
        std::shared_ptr<Colony> startingColony = std::make_shared<Colony>(planet);
        return startingColony;
    }

    void AddPlayer(std::shared_ptr<Galaxy> wholeGalaxy)
    {
        long id = player_id++;
        std::shared_ptr<Galaxy> startingGalaxy = GetStartingGalaxy(wholeGalaxy);
        std::shared_ptr<Colony> startingColony = GetStartingColony(startingGalaxy);
        std::shared_ptr<Player> new_player =
            std::make_shared<Player>(id, startingGalaxy, GetStartingResources(), startingColony);
        startingColony->owner = new_player;
        players[id] = new_player;
    }
};
