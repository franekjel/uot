#include "rendering_menu.h"
#include "client_context.h"
#include "game_gui.h"
#include "input_utilities.h"

void rendering::render_current_popup_menu(const client_context& context)
{
    auto& gr = context.gr;

    rendering::render_planet_helper(context, 1.0f, size_settings::popup_menu_area::width / 2,
                                    0.5f * planets_meta::frame_height, gr->planetTextures[planet_types::MENU_PLANET]);

    int i = 0;
    for (auto& elem : context.gui->popup_buttons)
    {
        rendering::render_button_sprite(elem, context);
    }
}

void rendering::render_menu_view::_draw(const client_context& context)
{
    render_background(context);
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

rendering::view_t rendering::render_menu_view::_up() { return std::make_shared<render_menu_view>(); }

rendering::view_t rendering::render_menu_view::_down() { return std::make_shared<render_universe_view>(); }

void rendering::render_menu_view::_mouse_handler(client_context& context, Uint32 event_type, SDL_MouseButtonEvent m,
                                                 int x, int y)
{
    namespace iu = input_utilities;
    const auto ev = iu::get_event_type(event_type, m, x, y);
    if (ev == iu::uot_event_type::left_click_resource || ev == iu::uot_event_type::left_click_context ||
        ev == iu::uot_event_type::left_click_play || ev == iu::uot_event_type::left_click_else)
    {
        auto& gs = context.gs;

        x = x - size_settings::popup_menu_area::x_offset;
        y = y - size_settings::popup_menu_area::y_offset;

        for (const auto& b : context.gui->popup_buttons)
        {
            if (iu::check_collision(x, y, b.x, b.y, b.w, b.h))
            {
                // calling a lambda, possibly losing a few instructions
                b.click_handler(context);
            }
        }
    }
    // motion in the popup buttons area
    else if (event_type == SDL_MOUSEMOTION && iu::check_view_area_collision<size_settings::popup_menu_area>(x, y))
    {
        using AreaType = size_settings::popup_menu_area;
        x = x - AreaType::x_offset;
        y = y - AreaType::y_offset;
        for (auto& b : context.gui->popup_buttons)
        {
            if (iu::check_collision(x, y, b.x, b.y, b.w, b.h))
            {
                context.gui->focused_button = b.button_id;
                return;
            }
        }
        context.gui->focused_button.reset();
    }
}

void rendering::render_menu_view::key_handler(client_context& context, Uint16 k)
{
    if (k == SDLK_ESCAPE)
    {
        context.view = _up();
    }
}
