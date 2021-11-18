#include <iostream>

#include "assets.h"
#include "game_renderer.h"
#include "game_gui.h"
#include <galaxygenerator.h>

void game_state_t::set_bk_texture(std::shared_ptr<SDL_Texture> bk) { bk_texture = bk; }

void game_state_t::set_renderer(std::shared_ptr<SDL_Renderer> r) { renderer = r; }

void game_state_t::set_window(std::shared_ptr<SDL_Window> w) { window = w; }

void game_state_t::draw()
{
    game_rendering::render_background(renderer);
    switch (game_view)
    {
        case game_view_t::menu_view:
            game_rendering::render_menu_view(renderer);
            break;
        case game_view_t::universe_view:
            game_rendering::render_universe_view(renderer);
            break;
        case game_view_t::galaxy_view:
            game_rendering::render_sector_view(renderer);
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


void game_state_t::reset_galaxy() {
    galaxy = GenerateGalaxy({generation_meta::num_sectors, generation_meta::multiplier});
}

void game_state_t::handleMouse(Uint32 event_type, SDL_MouseButtonEvent m, int x, int y) {
    if (sdl_utilities::check_view_area_collision<size_settings::play_area>(x, y)) {
        gui->template handleMouse<size_settings::play_area>(event_type, m, x, y);
        return;
    }

    if (sdl_utilities::check_view_area_collision<size_settings::button_area>(x, y)) {
        gui->template handleMouse<size_settings::button_area>(event_type, m, x, y);
        return;
    }

    if (sdl_utilities::check_view_area_collision<size_settings::context_area>(x, y)) {
        gui->template handleMouse<size_settings::context_area>(event_type, m, x, y);
        return;
    }

    if (sdl_utilities::check_view_area_collision<size_settings::window_area>(x, y)) {
        gui->template handleMouse<size_settings::window_area>(event_type, m, x, y);
        return;
    }

}

void game_state_t::set_gui(){
    gui = std::make_unique<game_gui>();
}
