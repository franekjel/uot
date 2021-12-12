#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <array>
#include <atomic>
#include <optional>
#include <variant>
#include <vector>
#include "../../libuot/galaxy.h"
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
    void reset_galaxy();
    std::optional<std::shared_ptr<Galaxy>> galaxy;
};

#endif
