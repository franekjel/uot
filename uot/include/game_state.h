#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <array>
#include <atomic>
#include <optional>
#include <vector>
#include "../../libuot/galaxy.h"
#include "sdl_utilities.h"
#include "singleton.h"
#include "uncopiable.h"

struct game_gui;

struct position
{
    float x;
    float y;
};

enum class game_view_t
{
    menu_view = 0,
    universe_view = 1,
    galaxy_view = 2,
    planet_view = 3,
    ship_view = 4
};

struct game_state_t : public uncopiable
{
   protected:
    game_state_t() {}
    std::atomic<game_view_t> game_view;
    friend singleton<game_state_t>;
    friend game_gui;

   public:
    void reset_galaxy();
    void set_view(game_view_t gv);
    game_view_t get_view();
    void set_gui();

    std::unique_ptr<game_gui> gui;
    ;
    std::optional<Galaxy> galaxy;

    int planet_frame = 0;
};

#endif
