#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <array>
#include <atomic>
#include <optional>
#include <variant>
#include <vector>
#include "../../libuot/sector.h"
#include "sdl_utilities.h"
#include "singleton.h"
#include "uncopiable.h"

struct position
{
    float x;
    float y;
};

struct game_state : public uncopiable
{
   protected:
    game_state() {}
    friend singleton<game_state>;

   public:
    void reset_sector();
    std::shared_ptr<Player> player;
    std::map<unsigned int, std::shared_ptr<Planet>> planets;
};

#endif
