#include "game_rendering.h"
#include <iostream>
#include "assets.h"
#include "game_gui.h"
#include "game_state.h"
#include "size_settings.h"

void game_rendering::render_background(const client_context& context)
{
    auto& r = context.r;
    auto& gr = context.gr;

    SDL_RenderSetViewport(r.get(), nullptr);
    SDL_RenderCopyEx(r.get(), gr.bk_texture.get(), nullptr, nullptr, 0, nullptr, SDL_FLIP_NONE);
}

void game_rendering::render_menu_view(const client_context& context)
{
    auto& r = context.r;
    auto& gr = context.gr;
    // draw the above astronaut buttons
    sdl_utilities::set_render_viewport<size_settings::button_area>(r.get());

    int i = 0;
    for (const auto elem : buttons_meta::menu_buttons)
    {
        game_rendering::render_button_sprite(elem, i++, context);
    }

    // =======================================================
    // draw the left, main game panel
    sdl_utilities::set_render_viewport<size_settings::play_area>(r.get());
    sdl_utilities::paint_frame(r.get(), SDL_Color{0xFF, 0xFF, 0xFF, 0xFF}, SDL_Color{0x00, 0x00, 0x00, 0x00});

    // render menu planet

    game_rendering::render_planet_helper(context, 1.0f, size_settings::play_area::width / 2,
                                         size_settings::play_area::height / 2,
                                         gr.planetTextures[planet_types::TERRAN_START_PLANET]);

    // =======================================================
    // draw the right, information / GUI panel
    sdl_utilities::set_render_viewport<size_settings::context_area>(r.get());
    sdl_utilities::paint_frame(r.get(), SDL_Color{0xFF, 0xFF, 0xFF, 0xFF}, SDL_Color{0x00, 0x00, 0x00, 0x00});
}

void game_rendering::render_button_sprite(const int button_id, const int ind, const client_context& context)
{
    auto& r = context.r;
    auto& gr = context.gr;
    auto& gs = context.gs;

    const auto texture = gr.buttonTextures[button_id];
    SDL_Rect s{0,
               (gs.gui->focused_button.has_value() && gs.gui->focused_button.value() == ind)
                   ? buttons_meta::button_texture_height
                   : 0,
               buttons_meta::button_texture_width, buttons_meta::button_texture_height};
    SDL_Rect d{buttons_meta::button_x_offset, buttons_meta::button_y_offset * ind, buttons_meta::button_width,
               buttons_meta::button_height};
    SDL_RenderCopyEx(r.get(), texture.get(), &s, &d, 0, nullptr, SDL_FLIP_NONE);
}

void game_rendering::render_planet_helper(const client_context& context, const float size_multiplier, const int x_off,
                                          const int y_off, const std::shared_ptr<SDL_Texture>& texture)
{
    auto& r = context.r;
    auto& gs = context.gs;

    SDL_Rect s{(gs.planet_frame / planets_meta::frame_duration) * planets_meta::frame_width, 0,
               planets_meta::frame_width, planets_meta::frame_height};

    SDL_Rect d{static_cast<int>(x_off - size_multiplier * 0.5f * planets_meta::frame_width),
               static_cast<int>(y_off - size_multiplier * 0.5f * planets_meta::frame_height),
               static_cast<int>(planets_meta::frame_width * size_multiplier),
               static_cast<int>(planets_meta::frame_height * size_multiplier)};

    SDL_RenderCopyEx(r.get(), texture.get(), &s, &d, 0, nullptr, SDL_FLIP_NONE);
}

void game_rendering::render_sector_selection(const client_context& context)
{
    static const auto tile_x = size_settings::play_area::width / generation_meta::sqrt_num_sectors;
    static const auto tile_y = size_settings::play_area::height / generation_meta::sqrt_num_sectors;

    auto& r = context.r;
    auto& gr = context.gr;
    auto& gs = context.gs;

    if (!gs.gui->current_sector.has_value())
    {
        return;
    }

    const auto curr = gs.gui->current_sector.value();

    const int x = size_settings::play_area::width * (0.8f * curr->position.x + 0.8f) / 2.0f;
    const int y = size_settings::play_area::height * (0.8f * curr->position.y + 0.8f) / 2.0f;
    SDL_Rect s{0, 0, selection_meta::texture_width, selection_meta::texture_height};

    SDL_Rect d{x, y, tile_x, tile_y};

    SDL_RenderCopyEx(r.get(), gr.selectionTextures[selection_types::SECTOR_SELECTION].get(), &s, &d,
                     SDL_GetTicks() / 100, NULL, SDL_FLIP_NONE);
}

void game_rendering::render_object_selection(const client_context& context)
{
    auto& gs = context.gs;
    auto& gr = context.gr;

    if (!gs.gui->current_object.has_value())
    {
        return;
    }

    auto& r = context.r;
    const auto& curr = gs.gui->current_object.value();

    const int x = size_settings::play_area::width * (curr->position.x + 1.0f) / 2;
    const int y = size_settings::play_area::height * (curr->position.y + 1.0f) / 2;

    SDL_Rect s{0, 0, selection_meta::texture_width, selection_meta::texture_height};

    SDL_Rect d{static_cast<int>(x - 0.5 * planets_meta::frame_width),
               static_cast<int>(y - 0.5 * planets_meta::frame_height), planets_meta::frame_width,
               planets_meta::frame_height};

    SDL_RenderCopyEx(r.get(), gr.selectionTextures[selection_types::SECTOR_SELECTION].get(), &s, &d,
                     SDL_GetTicks() / 100, NULL, SDL_FLIP_NONE);
}

void game_rendering::render_universe_view(const client_context& context)
{
    // draw the above astronaut buttons
    auto& gs = context.gs;
    auto& r = context.r;

    sdl_utilities::set_render_viewport<size_settings::button_area>(r.get());
    int i = 0;
    for (const auto elem : buttons_meta::universe_buttons)
    {
        game_rendering::render_button_sprite(elem, i++, context);
    }

    // =======================================================
    // draw the left, main game panel
    sdl_utilities::set_render_viewport<size_settings::play_area>(r.get());
    sdl_utilities::paint_frame(r.get(), SDL_Color{0xFF, 0xFF, 0xFF, 0xFF}, SDL_Color{0x00, 0x00, 0x00, 0x00});
    if (gs.galaxy.has_value())
    {
        // universe render here
        render_sector_selection(context);
        int u = 1;
        for (const auto& sector : gs.galaxy.value().sectors)
        {
            render_sector_helper<true>(context, sector);
        }
    }
    // =======================================================
    // draw the right, information / GUI panel
    sdl_utilities::set_render_viewport<size_settings::context_area>(r.get());
    sdl_utilities::paint_frame(r.get(), SDL_Color{0xFF, 0xFF, 0xFF, 0xFF}, SDL_Color{0x00, 0x00, 0x00, 0x00});
}

void game_rendering::render_sector_view(const client_context& context)
{
    // draw the above astronaut buttons
    auto& r = context.r;
    auto& gs = context.gs;

    sdl_utilities::set_render_viewport<size_settings::button_area>(r.get());

    int i = 0;
    for (const auto elem : buttons_meta::galaxy_buttons)
    {
        game_rendering::render_button_sprite(elem, i++, context);
    }

    // =======================================================
    // draw the left, main game panel
    sdl_utilities::set_render_viewport<size_settings::play_area>(r.get());
    sdl_utilities::paint_frame(r.get(), SDL_Color{0xFF, 0xFF, 0xFF, 0xFF}, SDL_Color{0x00, 0x00, 0x00, 0x00});
    render_object_selection(context);
    if (gs.gui->current_sector.has_value())
    {
        // sector render here
        const auto& curr = gs.gui->current_sector.value();
        render_sector_helper<false>(context, curr);
    }
    // =======================================================
    // draw the right, information / GUI panel
    sdl_utilities::set_render_viewport<size_settings::context_area>(r.get());
    sdl_utilities::paint_frame(r.get(), SDL_Color{0xFF, 0xFF, 0xFF, 0xFF}, SDL_Color{0x00, 0x00, 0x00, 0x00});
}

template <bool Universe>
void game_rendering::render_sector_helper(const client_context& context, const std::shared_ptr<Sector>& sector)
{
    static const auto tile_x = size_settings::play_area::width / generation_meta::sqrt_num_sectors;
    static const auto tile_y = size_settings::play_area::height / generation_meta::sqrt_num_sectors;

    auto& r = context.r;
    auto& gr = context.gr;
    // precompute
    if constexpr (Universe)
    {
        const auto corrected_x = (0.8f * sector->position.x + 0.8f) / 2.0f;
        const auto corrected_y = (0.8f * sector->position.y + 0.8f) / 2.0f;
        for (const auto& p : sector->objects)
        {
            const auto planet_x =
                (corrected_x * size_settings::play_area::width) + tile_x * (0.8f + 0.8f * p->position.x) * 0.5f;
            const auto planet_y =
                (corrected_y * size_settings::play_area::height) + tile_y * (0.8f + 0.8f * p->position.y) * 0.5f;
            render_planet_helper(context, 1.0f / generation_meta::sqrt_num_sectors, planet_x, planet_y,
                                 gr.planetTextures[planet_types::TERRAN_START_PLANET]);
        }
    }
    else
    {
        for (const auto& p : sector->objects)
        {
            const auto planet_x = size_settings::play_area::width * (1.0f + 1.0f * p->position.x) * 0.5f;
            const auto planet_y = size_settings::play_area::height * (1.0f + 1.0f * p->position.y) * 0.5f;
            render_planet_helper(context, 0.8, planet_x, planet_y,
                                 gr.planetTextures[planet_types::TERRAN_START_PLANET]);
        }
    }
}

void game_rendering::draw(client_context& context)
{
    auto& gs = context.gs;
    game_rendering::render_background(context);
    switch (gs.get_view())
    {
        case game_view_t::menu_view:
            render_menu_view(context);
            break;
        case game_view_t::universe_view:
            render_universe_view(context);
            break;
        case game_view_t::galaxy_view:
            render_sector_view(context);
            break;
        default:
            break;
    }

    SDL_RenderPresent(context.r.get());

    gs.planet_frame =
        (gs.planet_frame + 1 == (planets_meta::num_frames * planets_meta::frame_duration)) ? 0 : gs.planet_frame + 1;
}
