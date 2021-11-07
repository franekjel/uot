#include "../include/game_renderer.h"
#include "../include/size_settings.h"
#include "../include/assets.h"
#include "../include/game_resources.h"
#include "../include/game_state.h"

void game_rendering::render_background(const std::shared_ptr<SDL_Renderer>& r) {
    static const game_resources_t& gr = singleton<game_resources_t>::pointer();
    SDL_RenderSetViewport(r.get(), NULL);
    SDL_RenderCopyEx( r.get(), gr.bkTexture.get(), NULL, NULL, 0, NULL, SDL_FLIP_NONE );
}

void game_rendering::render_menu(const std::shared_ptr<SDL_Renderer>& r) {
    // draw the above astronaut buttons
    sdl_utilities::set_render_viewport<size_settings::button_area>(r.get());

    int i = 0;
    for(const auto elem : buttons_meta::menu_buttons) {
        game_rendering::render_button_sprite(elem, i++, r);
    }

    // =======================================================
    // draw the left, main game panel
    sdl_utilities::set_render_viewport<size_settings::play_area>(r.get());
    sdl_utilities::paint_frame(r.get(), SDL_Color {0xFF, 0xFF, 0xFF, 0xFF }, SDL_Color {0x00, 0x00, 0x00, 0x00 });

    // render menu planet

    game_rendering::render_menu_planet(r);

    // =======================================================
    // draw the right, information / GUI panel
    sdl_utilities::set_render_viewport<size_settings::context_area>(r.get());
    sdl_utilities::paint_frame(r.get(), SDL_Color {0xFF, 0xFF, 0xFF, 0xFF }, SDL_Color {0x00, 0x00, 0x00, 0x00 });

    // render menu planet
    //game_rendering::render_menu_planet(r);
}

void game_rendering::render_button_sprite(const int button_id, const int ind, const std::shared_ptr<SDL_Renderer>& r) {
    static const game_state_t& gs = singleton<game_state_t>::pointer();
    static const game_resources_t& gr = singleton<game_resources_t>::pointer();

    const auto texture = gr.buttonTextures[button_id];
    SDL_Rect s { 0, (gs.focused_button.has_value() && gs.focused_button.value() == ind) ? buttons_meta::button_texture_height : 0, buttons_meta::button_texture_width, buttons_meta::button_texture_height };
    SDL_Rect d { buttons_meta::button_x_offset, buttons_meta::button_y_offset * ind, buttons_meta::button_width, buttons_meta::button_height };
    SDL_RenderCopyEx( r.get(), texture.get(), &s, &d, 0, NULL, SDL_FLIP_NONE );
}

// the other planet rendering function will be a batch one, this is the ONLY
// case where we want to render one planet at the time. doesnt make sense to make
// this function more specific
void game_rendering::render_menu_planet(const std::shared_ptr<SDL_Renderer>& r) {
    static const game_state_t& gs = singleton<game_state_t>::pointer();
    static const game_resources_t& gr = singleton<game_resources_t>::pointer();
    static const auto texture = gr.planetTextures[ planet_types::TERRAN_START_PLANET ];

    // TODO: this should be abstracted away sdl_utilities, returning some kind of struct
    SDL_Rect vp;
    SDL_RenderGetViewport(r.get(), &vp);
    const auto vp_width = vp.w;
    const auto vp_height = vp.h;


    SDL_Rect s { (gs.planet_frame / planets_meta::frame_duration) * planets_meta::frame_width, 0, planets_meta::frame_width, planets_meta::frame_height};
    //SDL_Rect d { (vp_width - planets_meta::frame_width) / 2, (vp_height - planets_meta::frame_height) / 2, planets_meta::frame_width, planets_meta::frame_height };
    //SDL_RenderCopyEx(r.get(), texture.get(), &s, &d, 0, NULL, SDL_FLIP_NONE);

    for (position pos : gs.clicked_positions)
    {
        SDL_Rect d{static_cast<int>(pos.x) - planets_meta::frame_width / 2, static_cast<int>(pos.y) - planets_meta::frame_height / 2, planets_meta::frame_width, planets_meta::frame_height};
        SDL_RenderCopyEx(r.get(), texture.get(), &s, &d, 0, NULL, SDL_FLIP_NONE);
    }
}


void game_rendering::render_universe() {

}

void game_rendering::render_galaxy(const unsigned int galaxyIdx) {

}

void game_rendering::render_planet(const unsigned int planetIdx) {

}

void game_rendering::render_ship(const unsigned int shipIdx) {

}
