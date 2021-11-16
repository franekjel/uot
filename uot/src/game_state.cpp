#include <iostream>

#include "assets.h"
#include "game_renderer.h"
#include "game_state.h"

void game_state_t::set_bk_texture(std::shared_ptr<SDL_Texture> bk) { bk_texture = bk; }

void game_state_t::set_renderer(std::shared_ptr<SDL_Renderer> r) { renderer = r; }

void game_state_t::set_window(std::shared_ptr<SDL_Window> w) { window = w; }

void game_state_t::draw()
{
    game_rendering::render_background(renderer);
    switch (game_view)
    {
        case game_view_t::menu_view:
            game_rendering::render_menu(renderer);
            break;

        default:
            break;
    }

    SDL_RenderPresent(renderer.get());

    planet_frame =
        (planet_frame + 1 == (planets_meta::num_frames * planets_meta::frame_duration)) ? 0 : planet_frame + 1;
}

void game_state_t::set_view(game_view_t gv) { game_view.exchange(gv); }

std::shared_ptr<SDL_Renderer>& game_state_t::get_renderer() { return renderer; }

std::shared_ptr<SDL_Window>& game_state_t::get_window() { return window; }

void game_state_t::handleMouse(Uint32 event_type, int x, int y)
{
    switch (event_type)
    {
        case SDL_MOUSEMOTION:
            // check if menu area
            if (sdl_utilities::check_view_area_collision<size_settings::button_area>(x, y))
            {
                focused_button = (y - size_settings::button_area::y_offset) / buttons_meta::button_y_offset;
            }
            else
            {
                focused_button.reset();
            }
            break;

        case SDL_MOUSEBUTTONDOWN:
            if (sdl_utilities::check_view_area_collision<size_settings::play_area>(x, y))
            {
                size_settings::play_area play_area;
                position clickPosition;
                clickPosition.x = float(x - play_area.x_offset);
                clickPosition.y = float(y - play_area.y_offset);
                clicked_positions.push_back(clickPosition);
            }

            break;

        default:
            break;
    }
}
