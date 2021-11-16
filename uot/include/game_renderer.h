#ifndef GAMERENDERER_H
#define GAMERENDERER_H

#include <memory>
#include "sdl_utilities.h"

namespace game_rendering
{
void render_background(const std::shared_ptr<SDL_Renderer>& r);

void render_button_sprite(const int button_id, const int ind, const std::shared_ptr<SDL_Renderer>& r);

void render_menu_planet(const std::shared_ptr<SDL_Renderer>& r);

void render_menu(const std::shared_ptr<SDL_Renderer>& r);

void render_universe();

void render_galaxy(const unsigned int galaxy_idx);

void render_planet(const unsigned int planet_idx);

void render_ship(const unsigned int ship_idx);

void render_mouse_click(const std::shared_ptr<SDL_Renderer>& r);

}  // namespace game_rendering

#endif
