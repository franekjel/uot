#include "rendering/rendering_universe.h"
#include "client_context.h"
#include "game_gui.h"
#include "game_resources.h"
#include "game_state.h"
#include "player.h"
#include "utilities/input_utilities.h"

void rendering::render_universe_view::_wheel_handler(client_context& context, int x, int y, int xmov, int ymov) {}

void rendering::render_selected_sector_info(const client_context& context)
{
    const auto& r = context.r;
    const auto& gr = context.gr;
    const auto& gui = context.gui;

    auto sector_id = gui->current_sector.value()->sector_id % 10 + 1;

    sdl_utilities::render_text(r.get(), gr->main_font, "SECTOR NAME", size_settings::context_area::width / 2,
                               fonts::main_font_size / 2 + 30, size_settings::context_area::width - 50,
                               {0xFF, 0xFF, 0xFF, 0xFF});

    // render planet here again
    render_planet_helper(context, 2.0, size_settings::context_area::width / 2,
                         std::min(250, planets_meta::texture_size[sector_id] * 2), gr->planetTextures[sector_id]);

    // render planet info
    sdl_utilities::render_text(r.get(), gr->secondary_font,
                               " sector index: " + std::to_string(gui->current_sector.value()->sector_id) +
                                   "\n planet info 1\n planet info 2\n planet info 3",
                               size_settings::context_area::width / 2, size_settings::context_area::height * 0.75,
                               size_settings::context_area::width - 50, {0xFF, 0xFF, 0xFF, 0xFF});
}

void rendering::render_universe_view::_draw(client_context& context)
{
    render_background(context);
    // draw the above astronaut buttons
    auto& gr = context.gr;
    auto& r = context.r;
    auto& gui = context.gui;

    sdl_utilities::set_render_viewport<size_settings::resource_area>(r.get());
    sdl_utilities::paint_background(r.get(), SDL_Color{0x00, 0x00, 0x00, 150});
    render_resource_bar(context);

    auto state = context.getGameState();
    auto& gs = state.value;

    // draw the left, main game panel
    sdl_utilities::set_render_viewport<size_settings::play_area>(r.get());
    sdl_utilities::paint_frame_textured(r.get(), SDL_Color{0xFF, 0xFF, 0xFF, 0xFF}, gr->sky_texture);
    sdl_utilities::set_render_viewport<size_settings::play_area>(r.get());
    if (gs->player)
    {
        // universe render here
        for (const auto& sector : gs->player->known_galaxy->sectors)
        {
            render_sector_universe_helper(context, sector.second);
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
    else
    {
        for (auto& elem : context.gui->navigation_menu_buttons)
        {
            std::visit([&](auto&& v) { v->draw(context, context.gui->focused_button == v->button_id); }, elem);
        }
    }
}
void rendering::render_sector_selection(const client_context& context)
{
    const auto& r = context.r;
    const auto& gr = context.gr;
    const auto& gui = context.gui;

    if (!gui->current_sector.has_value())
    {
        return;
    }

    sdl_utilities::set_custom_viewport<size_settings::play_area, size_settings::frame_size>(r.get());
    sdl_utilities::paint_background(r.get(), SDL_Color{0x00, 0x00, 0x00, 150});
    sdl_utilities::set_render_viewport<size_settings::play_area>(r.get());

    const auto curr = gui->current_sector.value();
    const int tex_size = planets_meta::texture_size[SECTOR_1] * planets_meta::sector_multiplier;
    const int x = size_settings::play_area::width * (0.5f * curr->position.x + 0.5f) - tex_size / 2;
    const int y = size_settings::play_area::height * (0.5f * curr->position.y + 0.5f) - tex_size / 2;
    SDL_Rect s{0, 0, selection_meta::texture_width, selection_meta::texture_height};

    SDL_Rect d{x, y, tex_size, tex_size};

    SDL_RenderCopyEx(r.get(), gr->selectionTextures[selection_types::SECTOR_SELECTION].get(), &s, &d,
                     SDL_GetTicks() / 100, NULL, SDL_FLIP_NONE);
    render_sector_universe_helper(context, gui->current_sector.value());
}

void rendering::render_sector_universe_helper(const client_context& context, const std::shared_ptr<Sector>& sector)
{
    const auto& gr = context.gr;
    const auto& gui = context.gui;

    const auto corrected_x = size_settings::play_area::width * (0.5f + 0.5f * sector->position.x);
    const auto corrected_y = size_settings::play_area::height * (0.5f + 0.5f * sector->position.y);
    render_planet_helper(context, planets_meta::sector_multiplier, corrected_x, corrected_y,
                         gr->planetTextures[gui->galaxy_stars_textures.at(sector->sector_id)]);

    SDL_SetRenderDrawColor(context.r.get(), 0xFF, 0xFF, 0xFF, 0xFF);

    for (const auto& n : sector->neighbors)
    {
        const auto n_x = size_settings::play_area::width * (0.5f + 0.5f * n->position.x);
        const auto n_y = size_settings::play_area::height * (0.5f + 0.5f * n->position.y);
        SDL_RenderDrawLine(context.r.get(), corrected_x, corrected_y, n_x, n_y);
    }
}

rendering::view_t rendering::render_universe_view::_up() { return std::make_shared<render_menu_view>(); }

rendering::view_t rendering::render_universe_view::_down(client_context& context)
{
    return std::make_shared<render_sector_view>();
}

void rendering::render_universe_view::_mouse_handler(client_context& context, Uint32 event_type, SDL_MouseButtonEvent m,
                                                     int x, int y)
{
    namespace iu = input_utilities;
    const auto et = iu::get_event_type(event_type, m, x, y);
    if (et == iu::uot_event_type::left_click_play)
    {
        using AreaType = size_settings::play_area;
        auto state = context.getGameState();
        auto& gs = state.value;
        auto& gr = context.gr;
        auto& current_sector = context.gui->current_sector;
        if (!gs->player)
        {
            return;
        }
        x = x - AreaType::x_offset;
        y = y - AreaType::y_offset;
        const int sprite_off = planets_meta::texture_size[SECTOR_1] * planets_meta::sector_multiplier * 0.5f;
        for (const auto& [id, s] : gs->player->known_galaxy->sectors)
        {
            if (iu::check_collision(x, y, size_settings::play_area::width * (0.5f + 0.5f * s->position.x) - sprite_off,
                                    size_settings::play_area::height * (0.5f + 0.5f * s->position.y) - sprite_off,
                                    planets_meta::texture_size[SECTOR_1] * planets_meta::sector_multiplier,
                                    planets_meta::texture_size[SECTOR_1] * planets_meta::sector_multiplier))
            {
                if (current_sector.has_value() && current_sector.value()->sector_id == s->sector_id)
                {
                    context.view = _down(context);
                    if (Mix_PlayChannel(-1, context.gr->open_planet.get(), 0) == -1)
                    {
                        throw std::runtime_error("couldnt plat the click sound");
                    }
                }
                current_sector = s;
                return;
            }
        }
        current_sector.reset();
    }
    else if (et == iu::uot_event_type::left_click_context || et == iu::uot_event_type::motion_context)
    {
        x = x - size_settings::context_area::x_offset;
        y = y - size_settings::context_area::y_offset;

        bool hit = false;
        for (const auto& b : context.gui->navigation_menu_buttons)
        {
            std::visit(
                [&](auto&& v)
                {
                    const auto& pos = v->pos;
                    if (iu::check_collision(x, y, pos.x, pos.y, pos.w, pos.h))
                    {
                        // calling a lambda, possibly losing a few instructions
                        if (event_type == SDL_MOUSEBUTTONDOWN)
                        {
                            v->clicked(context);
                            return;
                        }

                        context.gui->focused_button = v->button_id;
                        hit = true;
                    }
                },
                b);
        }

        if (!hit)
            context.gui->focused_button.reset();
    }
}

void rendering::render_universe_view::key_handler(client_context& context, Uint16 k)
{
    if (k == SDLK_ESCAPE)
    {
        context.gui->current_sector.reset();
        context.view = _up();
    }
}
