#include "../include/game_state.h"
#include "../include/game_renderer.h"
#include "../include/assets.h"
#include <iostream>

void game_state_t::set_bk_texture(std::shared_ptr<SDL_Texture> bk) {
    bk_texture = bk;
}

void game_state_t::set_renderer(std::shared_ptr<SDL_Renderer> r) {
    renderer = r;
}

void game_state_t::set_window(std::shared_ptr<SDL_Window> w) {
    window = w;
}

void game_state_t::draw() {
    game_rendering::render_background(renderer);
    switch (game_view)
    {
    case game_view_t::menu_view:
        game_rendering::render_menu(renderer);
        break;
    
    default:
        break;
    }

	SDL_RenderPresent( renderer.get() );

}


void game_state_t::set_view(game_view_t gv) {
    game_view.exchange(gv);
}

std::shared_ptr<SDL_Renderer>& game_state_t::get_renderer() {
    return renderer;
}

std::shared_ptr<SDL_Window>& game_state_t::get_window() {
    return window;
}