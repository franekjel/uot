#include "rendering_menu.h"
#include "client_context.h"
#include "game_gui.h"
#include "game_resources.h"
#include "game_state.h"
#include "input_utilities.h"

void rendering::render_menu_view::_wheel_handler(client_context& context,
        int x, int y, int xmov, int ymov) { }

void rendering::render_current_popup_menu(client_context& context)
{
    auto& gr = context.gr;

    rendering::render_planet_helper(context, 1.0f, size_settings::popup_menu_area::width / 2,
                                    0.5f * planets_meta::frame_height, gr->planetTextures[planet_types::MENU_PLANET]);

    int i = 0;
    for (auto& elem : context.gui->popup_buttons)
    {
        std::visit([&](auto&& v) { v->draw(context, context.gui->focused_button == v->button_id); }, elem);
    }
}

void rendering::render_menu_view::_draw(client_context& context)
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
        x = x - size_settings::popup_menu_area::x_offset;
        y = y - size_settings::popup_menu_area::y_offset;

        for (const auto& b : context.gui->popup_buttons)
        {
            std::visit(
                [&](auto&& v)
                {
                    const auto& pos = v->pos;
                    if (iu::check_collision(x, y, pos.x, pos.y, pos.w, pos.h))
                    {
                        // calling a lambda, possibly losing a few instructions
                        v->clicked(context);
                    }
                },
                b);
        }
    }
    // motion in the popup buttons area
    else if (event_type == SDL_MOUSEMOTION && iu::check_view_area_collision<size_settings::popup_menu_area>(x, y))
    {
        using AreaType = size_settings::popup_menu_area;
        x = x - AreaType::x_offset;
        y = y - AreaType::y_offset;
        bool hit = false;
        for (auto& b : context.gui->popup_buttons)
        {
            std::visit(
                [&](auto&& v)
                {
                    const auto& pos = v->pos;
                    if (iu::check_collision(x, y, pos.x, pos.y, pos.w, pos.h))
                    {
                        context.gui->focused_button = v->button_id;
                        hit = true;
                        return;
                    }
                },
                b);
        }
        if (!hit)
        {
            context.gui->focused_button.reset();
        }
    }
}

void rendering::render_menu_view::key_handler(client_context& context, Uint16 k)
{
    if (k == SDLK_ESCAPE)
    {
        context.view = _up();
    }
}
