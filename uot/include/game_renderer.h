#ifndef GAMERENDERER_H
#define GAMERENDERER_H

#include <memory>
#include "sdl_utilities.h"
#include <sector.h>


namespace game_rendering
{

void render_background(const std::shared_ptr<SDL_Renderer>& r);

void render_button_sprite(const int button_id, const int ind, const std::shared_ptr<SDL_Renderer>& r);

void render_planet_helper(const std::shared_ptr<SDL_Renderer>& r, const float size_multiplier,
                                          const int x_off, const int y_off, const std::shared_ptr<SDL_Texture>& texture);

void render_menu_view(const std::shared_ptr<SDL_Renderer>& r);

void render_universe_view(const std::shared_ptr<SDL_Renderer>& r);

template<bool Universe>
void render_sector_helper(const std::shared_ptr<SDL_Renderer>& r, const std::shared_ptr<Sector>& sector);

void render_sector_view(const Sector& sector);

void render_sector_object();

void render_mouse_click(const std::shared_ptr<SDL_Renderer>& r);

void render_sector_selection(const std::shared_ptr<SDL_Renderer>& r);

void render_sector_view(const std::shared_ptr<SDL_Renderer>& r);

void render_object_selection(const std::shared_ptr<SDL_Renderer>& r);
}  // namespace game_rendering

#endif
