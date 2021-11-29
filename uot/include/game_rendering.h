#ifndef GAMERENDERER_H
#define GAMERENDERER_H

#include <memory>
#include "../../libuot/sector.h"
#include "client_context.h"
#include "game_state.h"
#include "resource_manager.h"
#include "sdl_utilities.h"

namespace game_rendering
{
void draw(client_context& context);

void render_background(const client_context& context);

void render_button_sprite(const int button_id, const int ind, const client_context& context);

void render_planet_helper(const client_context& context, const float size_multiplier, const int x_off, const int y_off,
                          const std::shared_ptr<SDL_Texture>& texture);
void render_menu_view(const client_context& context);

void render_universe_view(const client_context& context);

template <bool Universe>
void render_sector_helper(const client_context& context, const std::shared_ptr<Sector>& sector);

void render_sector_view(const Sector& sector);

void render_sector_object();

void render_mouse_click(const client_context& context);

void render_sector_selection(const client_context& context);

void render_sector_view(const client_context& context);

void render_object_selection(const client_context& context);

}  // namespace game_rendering

#endif
