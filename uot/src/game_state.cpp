#include <iostream>

#include <galaxygenerator.h>
#include "assets.h"
#include "game_gui.h"
#include "game_rendering.h"

void game_state_t::set_view(game_view_t gv) { game_view.exchange(gv); }

game_view_t game_state_t::get_view() { return game_view; }

void game_state_t::reset_galaxy()
{
    galaxy = GenerateGalaxy({generation_meta::num_sectors, generation_meta::multiplier});
}

void game_state_t::set_gui() { gui = std::make_unique<game_gui>(); }
