#include "rendering_universe.h"
#include "client_context.h"
#include "game_gui.h"
#include "input_utilities.h"

void rendering::render_selected_sector_info(const client_context& context)
{
    auto& r = context.r;
    auto& gs = context.gs;
    auto& gr = context.gr;
    auto& gui = context.gui;

    auto sector_id = gui->current_sector.value()->sector_id % 10 + 1;
    sdl_utilities::render_text(r.get(), gr.main_font, "SECTOR NAME", 30, 10, size_settings::context_area::width - 50,
                               {0xFF, 0xFF, 0xFF, 0xFF});

    // render planet here again
    render_planet_helper(context, 2.0, size_settings::context_area::width / 2,
                         std::min(250, planets_meta::texture_size[sector_id] * 2), gr.planetTextures[sector_id]);

    // render planet info
    sdl_utilities::render_text(r.get(), gr.secondary_font,
                               " sector index: " + std::to_string(gui->current_sector.value()->sector_id) +
                                   "\n sector info 1\n sector info 2\n sector info 3",
                               40, 450, size_settings::context_area::width - 50, {0xFF, 0xFF, 0xFF, 0xFF});
}

void rendering::render_universe_view::_draw(const client_context& context)
{
    render_background(context);
    // draw the above astronaut buttons
    auto& gs = context.gs;
    auto& gr = context.gr;
    auto& r = context.r;
    auto& gui = context.gui;

    sdl_utilities::set_render_viewport<size_settings::resource_area>(r.get());
    render_resource_bar(context);

    // draw the left, main game panel
    sdl_utilities::set_render_viewport<size_settings::play_area>(r.get());
    sdl_utilities::paint_frame_textured(r.get(), SDL_Color{0xFF, 0xFF, 0xFF, 0xFF}, gr.sky_texture);
    sdl_utilities::set_render_viewport<size_settings::play_area>(r.get());
    if (gs.galaxy.has_value())
    {
        // universe render here
        int u = 1;
        for (const auto& sector : gs.galaxy.value()->sectors)
        {
            render_sector_universe_helper(context, sector);
        }
        render_sector_selection(context);
    }
    // =======================================================
    // draw the right, information / GUI panel
    sdl_utilities::set_render_viewport<size_settings::context_area>(r.get());
    sdl_utilities::paint_frame(r.get(), SDL_Color{0xFF, 0xFF, 0xFF, 0xFF}, SDL_Color{0x00, 0x00, 0x00, 0xFF});

    if (gui->current_sector.has_value())
    {
        rendering::render_selected_sector_info(context);
    }

    sdl_utilities::set_render_viewport<size_settings::resource_area>(r.get());
    sdl_utilities::paint_background(r.get(), SDL_Color{0x00, 0x00, 0x00, 150});
    render_resource_bar(context);
}
void rendering::render_sector_selection(const client_context& context)
{
    static const auto tile_x = size_settings::play_area::width / generation_meta::sqrt_num_sectors;
    static const auto tile_y = size_settings::play_area::height / generation_meta::sqrt_num_sectors;

    auto& r = context.r;
    auto& gr = context.gr;
    auto& gs = context.gs;
    auto& gui = context.gui;

    if (!gui->current_sector.has_value())
    {
        return;
    }

    sdl_utilities::paint_background(r.get(), SDL_Color{0x00, 0x00, 0x00, 150});

    const auto curr = gui->current_sector.value();
    const int tex_size = planets_meta::texture_size[SECTOR_1] * planets_meta::sector_multiplier;
    const int x = size_settings::play_area::width * (0.5f * curr->position.x + 0.5f) - tex_size / 2;
    const int y = size_settings::play_area::height * (0.5f * curr->position.y + 0.5f) - tex_size / 2;
    SDL_Rect s{0, 0, selection_meta::texture_width, selection_meta::texture_height};

    SDL_Rect d{x, y, tex_size, tex_size};

    SDL_RenderCopyEx(r.get(), gr.selectionTextures[selection_types::SECTOR_SELECTION].get(), &s, &d,
                     SDL_GetTicks() / 100, NULL, SDL_FLIP_NONE);
    render_sector_universe_helper(context, gui->current_sector.value());
}

void rendering::render_sector_universe_helper(const client_context& context, const std::shared_ptr<Sector>& sector)
{
    auto& r = context.r;
    auto& gr = context.gr;

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

rendering::view_t rendering::render_universe_view::_up() { return std::make_shared<render_menu_view>(); }

rendering::view_t rendering::render_universe_view::_down() { return std::make_shared<render_sector_view>(); }

void rendering::render_universe_view::_mouse_handler(client_context& context, Uint32 event_type, SDL_MouseButtonEvent m,
                                                     int x, int y)
{
    namespace iu = input_utilities;
    const auto et = iu::get_event_type(event_type, m, x, y);
    if (et == iu::uot_event_type::left_click_play)
    {
        using AreaType = size_settings::play_area;
        auto& gs = context.gs;
        auto& gr = context.gr;
        auto& current_sector = context.gui->current_sector;

        x = x - AreaType::x_offset;
        y = y - AreaType::y_offset;
        const int sprite_off = planets_meta::texture_size[SECTOR_1] * planets_meta::sector_multiplier * 0.5f;
        for (const auto s : gs.galaxy.value()->sectors)
        {
            if (iu::check_collision(x, y, size_settings::play_area::width * (0.5f + 0.5f * s->position.x) - sprite_off,
                                    size_settings::play_area::height * (0.5f + 0.5f * s->position.y) - sprite_off,
                                    planets_meta::texture_size[SECTOR_1] * planets_meta::sector_multiplier,
                                    planets_meta::texture_size[SECTOR_1] * planets_meta::sector_multiplier))
            {
                if (current_sector.has_value() && current_sector.value()->sector_id == s->sector_id)
                {
                    context.view = _down();
                }
                current_sector = s;
                return;
            }
        }
        current_sector.reset();
    }
}

void rendering::render_universe_view::key_handler(client_context& context, Uint16 k)
{
    if (k == SDLK_ESCAPE)
    {
        context.view = _up();
    }
}
