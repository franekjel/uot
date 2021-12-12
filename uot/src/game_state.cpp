#include <iostream>

#include <galaxygenerator.h>
#include "assets.h"
#include "game_state.h"

void game_state::reset_galaxy()
{
    galaxy = std::make_shared<Galaxy>(GenerateGalaxy({generation_meta::num_sectors, generation_meta::multiplier}));
}
