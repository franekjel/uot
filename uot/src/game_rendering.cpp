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

void game_rendering::render_current_popup_menu(const client_context& context)
{
    auto& gr = context.gr;

    game_rendering::render_planet_helper(context, 1.0f, size_settings::popup_menu_area::width / 2,
                                         0.5f * planets_meta::frame_height,
                                         gr.planetTextures[planet_types::MENU_PLANET]);

    int i = 0;
    for (auto& elem : context.gs.gui->popup_buttons)
    {
        game_rendering::render_button_sprite(elem, context);
    }
}

void game_rendering::render_menu_view(const client_context& context)
{
    auto& r = context.r;
    auto& gr = context.gr;

    // =======================================================
    // draw the menu on top of previous render

    sdl_utilities::set_render_viewport<size_settings::window_area>(r.get());
    sdl_utilities::paint_background(r.get(), SDL_Color{0x00, 0x00, 0x00, 150});

    sdl_utilities::set_render_viewport<size_settings::popup_menu_area>(r.get());
    sdl_utilities::paint_frame(r.get(), SDL_Color{0xFF, 0xFF, 0xFF, 0xFF}, SDL_Color{0x00, 0x00, 0x00, 0xFF});
    render_current_popup_menu(context);
}

void game_rendering::render_button_sprite(ui_button& button, const client_context& context)
{
    auto& r = context.r;
    auto& gr = context.gr;
    auto& gs = context.gs;

    const auto texture = gr.buttonTextures[button.type];
    SDL_Rect s{0,
               (gs.gui->focused_button.has_value() && gs.gui->focused_button.value() == button.button_id)
                   ? buttons_meta::button_texture_height
                   : 0,
               buttons_meta::button_texture_width, buttons_meta::button_texture_height};
    SDL_Rect d{button.x, button.y, button.w, button.h};
    SDL_RenderCopyEx(r.get(), texture.get(), &s, &d, 0, nullptr, SDL_FLIP_NONE);
}

void game_rendering::render_planet_helper(const client_context& context, const float size_multiplier, const int x_off,
                                          const int y_off, const texture_t& tex)
{
    auto& r = context.r;
    auto& gs = context.gs;

    SDL_Rect s{(gs.planet_frame / planets_meta::frame_duration) * tex.w, 0, tex.w, tex.h};

    SDL_Rect d{static_cast<int>(x_off - size_multiplier * 0.5f * tex.w),
               static_cast<int>(y_off - size_multiplier * 0.5f * tex.h), static_cast<int>(tex.w * size_multiplier),
               static_cast<int>(tex.h * size_multiplier)};

    SDL_RenderCopyEx(r.get(), tex.t.get(), &s, &d, 0, nullptr, SDL_FLIP_NONE);
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

    sdl_utilities::paint_background(r.get(), SDL_Color{0x00, 0x00, 0x00, 150});

    const auto curr = gs.gui->current_sector.value();

    const int x =
        size_settings::play_area::width * (0.5f * curr->position.x + 0.5f) - planets_meta::texture_size[SECTOR_1] / 2;
    const int y =
        size_settings::play_area::height * (0.5f * curr->position.y + 0.5f) - planets_meta::texture_size[SECTOR_1] / 2;
    SDL_Rect s{0, 0, selection_meta::texture_width, selection_meta::texture_height};

    SDL_Rect d{x, y, planets_meta::texture_size[SECTOR_1], planets_meta::texture_size[SECTOR_1]};

    SDL_RenderCopyEx(r.get(), gr.selectionTextures[selection_types::SECTOR_SELECTION].get(), &s, &d,
                     SDL_GetTicks() / 100, NULL, SDL_FLIP_NONE);
    render_sector_helper<true>(context, gs.gui->current_sector.value());
}

void game_rendering::render_object_selection(const client_context& context)
{
    auto& r = context.r;
    auto& gs = context.gs;
    auto& gr = context.gr;

    if (!gs.gui->current_object.has_value())
    {
        return;
    }

    sdl_utilities::paint_background(r.get(), SDL_Color{0x00, 0x00, 0x00, 150});

    const auto& curr = gs.gui->current_object.value();

    const int x = size_settings::play_area::width * (curr->position.x + 1.0f) / 2;
    const int y = size_settings::play_area::height * (curr->position.y + 1.0f) / 2;

    SDL_Rect s{0, 0, selection_meta::texture_width, selection_meta::texture_height};

    SDL_Rect d{static_cast<int>(x - 0.5 * planets_meta::frame_width),
               static_cast<int>(y - 0.5 * planets_meta::frame_height), planets_meta::frame_width,
               planets_meta::frame_height};

    SDL_RenderCopyEx(r.get(), gr.selectionTextures[selection_types::SECTOR_SELECTION].get(), &s, &d,
                     SDL_GetTicks() / 100, NULL, SDL_FLIP_NONE);

    render_planet_helper(context, 0.8, x, y, gr.planetTextures[11 + curr->id % 18]);
}

void game_rendering::render_selected_object_info(const client_context& context)
{
    auto& r = context.r;
    auto& gs = context.gs;
    auto& gr = context.gr;

    auto object_id = GAS_GIANT_1 + gs.gui->current_object.value()->id % (planets_meta::num_planets - GAS_GIANT_1);
    sdl_utilities::render_text(r.get(), gr.main_font, "PLANET NAME", 30, 10, size_settings::context_area::width - 50,
                               {0xFF, 0xFF, 0xFF, 0xFF});

    // render planet here again
    render_planet_helper(context, 2.0, size_settings::context_area::width / 2,
                         std::min(250, planets_meta::texture_size[object_id] * 2), gr.planetTextures[object_id]);

    // render planet info
    sdl_utilities::render_text(r.get(), gr.secondary_font,
                               " planet index: " + std::to_string(gs.gui->current_object.value()->id) +
                                   "\n planet info 1\n planet info 2\n planet info 3",
                               40, 450, size_settings::context_area::width - 50, {0xFF, 0xFF, 0xFF, 0xFF});
}

void game_rendering::render_selected_sector_info(const client_context& context)
{
    auto& r = context.r;
    auto& gs = context.gs;
    auto& gr = context.gr;

    auto sector_id = gs.gui->current_sector.value()->sector_id % 10 + 1;
    sdl_utilities::render_text(r.get(), gr.main_font, "SECTOR NAME", 30, 10, size_settings::context_area::width - 50,
                               {0xFF, 0xFF, 0xFF, 0xFF});

    // render planet here again
    render_planet_helper(context, 2.0, size_settings::context_area::width / 2,
                         std::min(250, planets_meta::texture_size[sector_id] * 2), gr.planetTextures[sector_id]);

    // render planet info
    sdl_utilities::render_text(r.get(), gr.secondary_font,
                               " sector index: " + std::to_string(gs.gui->current_sector.value()->sector_id) +
                                   "\n sector info 1\n sector info 2\n sector info 3",
                               40, 450, size_settings::context_area::width - 50, {0xFF, 0xFF, 0xFF, 0xFF});
}

void game_rendering::render_universe_view(const client_context& context)
{
    // draw the above astronaut buttons
    auto& gs = context.gs;
    auto& gr = context.gr;
    auto& r = context.r;

    sdl_utilities::set_render_viewport<size_settings::resource_area>(r.get());

    // =======================================================
    // draw the left, main game panel
    sdl_utilities::set_render_viewport<size_settings::play_area>(r.get());
    sdl_utilities::paint_frame_textured(r.get(), SDL_Color{0xFF, 0xFF, 0xFF, 0xFF}, gr.sky_texture);
    sdl_utilities::set_render_viewport<size_settings::play_area>(r.get());
    if (gs.galaxy.has_value())
    {
        // universe render here
        int u = 1;
        for (const auto& sector : gs.galaxy.value().sectors)
        {
            render_sector_helper<true>(context, sector);
        }
        render_sector_selection(context);
    }
    // =======================================================
    // draw the right, information / GUI panel
    sdl_utilities::set_render_viewport<size_settings::context_area>(r.get());
    sdl_utilities::paint_frame(r.get(), SDL_Color{0xFF, 0xFF, 0xFF, 0xFF}, SDL_Color{0x00, 0x00, 0x00, 0xFF});

    if (gs.gui->current_sector.has_value())
    {
        game_rendering::render_selected_sector_info(context);
    }

    sdl_utilities::set_render_viewport<size_settings::resource_area>(r.get());
    sdl_utilities::paint_background(r.get(), SDL_Color{0x00, 0x00, 0x00, 150});
    render_resource_bar(context);
}

void game_rendering::render_resource_bar(const client_context& context)
{
    constexpr std::array<int, 8> positions{0,
                                           0,
                                           resources_meta::single_size,
                                           0,
                                           0,
                                           resources_meta::single_size,
                                           resources_meta::single_size,
                                           resources_meta::single_size};
    constexpr std::array<int, 4> amounts{45, 87, 563, 2};
    constexpr int num_resources = 4;
    int x_off = 20;
    constexpr int y_off = (size_settings::resource_area::height - fonts::resource_font_size) / 2;
    for (int i = 0; i < num_resources; ++i)
    {
        SDL_Rect s{positions[i], positions[i + 1], resources_meta::single_size, resources_meta::single_size};
        SDL_Rect d{x_off, y_off, fonts::resource_font_size, fonts::resource_font_size};
        SDL_RenderCopyEx(context.r.get(), context.gr.resource_texture.get(), &s, &d, 0, nullptr, SDL_FLIP_NONE);

        x_off += fonts::resource_font_size;
        sdl_utilities::render_text(context.r.get(), context.gr.resource_font, ": " + std::to_string(amounts[i]), x_off,
                                   y_off, 150, {0xFF, 0xFF, 0xFF, 0xFF});
        x_off += 180;
    }
}

void game_rendering::render_sector_view(const client_context& context)
{
    // draw the above astronaut buttons
    auto& r = context.r;
    auto& gs = context.gs;
    auto& gr = context.gr;

    sdl_utilities::set_render_viewport<size_settings::resource_area>(r.get());

    // =======================================================
    // draw the left, main game panel
    sdl_utilities::set_render_viewport<size_settings::play_area>(r.get());
    sdl_utilities::paint_frame_textured(r.get(), SDL_Color{0xFF, 0xFF, 0xFF, 0xFF}, gr.sky_texture);
    sdl_utilities::set_render_viewport<size_settings::play_area>(r.get());
    if (gs.gui->current_sector.has_value())
    {
        // sector render here
        const auto& curr = gs.gui->current_sector.value();
        render_sector_helper<false>(context, curr);
    }
    render_object_selection(context);
    // =======================================================
    // draw the right, information / GUI panel
    sdl_utilities::set_render_viewport<size_settings::context_area>(r.get());
    sdl_utilities::paint_frame(r.get(), SDL_Color{0xFF, 0xFF, 0xFF, 0xFF}, SDL_Color{0x00, 0x00, 0x00, 0xFF});

    if (gs.gui->current_object.has_value())
    {
        render_selected_object_info(context);
    }

    sdl_utilities::set_render_viewport<size_settings::resource_area>(r.get());
    sdl_utilities::paint_background(r.get(), SDL_Color{0x00, 0x00, 0x00, 150});
    render_resource_bar(context);
}

template <bool Universe>
void game_rendering::render_sector_helper(const client_context& context, const std::shared_ptr<Sector>& sector)
{
    auto& r = context.r;
    auto& gr = context.gr;

    if constexpr (Universe)
    {
        const auto corrected_x = size_settings::play_area::width * (0.5f + 0.5f * sector->position.x);
        const auto corrected_y = size_settings::play_area::height * (0.5f + 0.5f * sector->position.y);
        render_planet_helper(context, planets_meta::sector_multiplier, corrected_x, corrected_y,
                             gr.planetTextures[sector->sector_id % 10 + 1]);

        SDL_SetRenderDrawColor(context.r.get(), 0xFF, 0xFF, 0xFF, 0xFF);

        for (const auto& n : sector->neighbors)
        {
            const auto n_x = size_settings::play_area::width * (0.5f + 0.5f * n->position.x);
            const auto n_y = size_settings::play_area::height * (0.5f + 0.5f * n->position.y);
            SDL_RenderDrawLine(context.r.get(), corrected_x, corrected_y, n_x, n_y);
        }
    }
    else
    {
        int i = 0;
        for (const auto& p : sector->objects)
        {
            const auto planet_x = size_settings::play_area::width * (1.0f + 1.0f * p->position.x) * 0.5f;
            const auto planet_y = size_settings::play_area::height * (1.0f + 1.0f * p->position.y) * 0.5f;
            render_planet_helper(context, 0.8, planet_x, planet_y, gr.planetTextures[11 + p->id % 18]);
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
