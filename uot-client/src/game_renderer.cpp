#include "../include/game_renderer.h"
#include "../include/size_settings.h"
#include "../include/assets.h"
#include "../include/game_resources.h"

void game_rendering::render_background(const std::shared_ptr<SDL_Renderer>& r) {
    static const game_resources_t& gr = singleton<game_resources_t>::pointer();
    SDL_RenderSetViewport(r.get(), NULL);
    SDL_RenderCopyEx( r.get(), gr.bkTexture.get(), NULL, NULL, 0, NULL, SDL_FLIP_NONE );
}

void game_rendering::render_menu(const std::shared_ptr<SDL_Renderer>& r) {
    // draw the above astronaut buttons
    sdl_utilities::set_render_viewport<size_settings::button_area>(r.get());
    sdl_utilities::paint_background(r.get(), SDL_Color {0xFF, 0xFF, 0xFF, 0xFF });

    int i = 0;
    for(const auto elem : button_sets::menu_buttons) {
        game_rendering::render_button(elem, i++, r);
    }

    // =======================================================
    // draw the left, main game panel
    sdl_utilities::set_render_viewport<size_settings::play_area>(r.get());
    sdl_utilities::paint_background(r.get(), SDL_Color {0xFF, 0xFF, 0xFF, 0xFF });
    // RENDER HERE

    // =======================================================
    // draw the right, information / GUI panel
    sdl_utilities::set_render_viewport<size_settings::context_area>(r.get());
    sdl_utilities::paint_background(r.get(), SDL_Color {0xFF, 0xFF, 0xFF, 0xFF });
}

void game_rendering::render_button(const int button_id, const int ind, const std::shared_ptr<SDL_Renderer>& r) {
    static const game_resources_t& gr = singleton<game_resources_t>::pointer();
    const auto texture = gr.bkTexture;
    SDL_Rect s { 0, 0, 100, 100 };
    SDL_Rect d { 0, 110 * ind, 300, 100 };
    SDL_RenderCopyEx( r.get(), texture.get(), &s, &d, 0, NULL, SDL_FLIP_NONE );
}


void game_rendering::render_universe() {

}

void game_rendering::render_galaxy(const unsigned int galaxyIdx) {

}

void game_rendering::render_planet(const unsigned int planetIdx) {

}

void game_rendering::render_ship(const unsigned int shipIdx) {

}