#ifndef GAME_GUI_H
#define GAME_GUI_H

#include <sector.h>
#include <sectorobject.h>
#include <iostream>
#include "assets.h"
#include "client_context.h"
#include "game_state.h"
#include "ui_button.h"
#include "ui_menu_lists.h"

enum class MouseButton
{
    MOUSE_LEFT,
    MOUSE_RIGHT
};

enum class click_type
{
    menu_view_click,

    universe_view_resource_click,
    universe_view_play_click,
    universe_view_context_click,
    universe_view_other_click,

    galaxy_view_resource_click,
    galaxy_view_play_click,
    galaxy_view_context_click,
    galaxy_view_other_click,

    planet_view_resource_click,
    planet_view_play_click,
    planet_view_context_click,
    planet_view_other_click,

    other
};

template <typename AreaType, game_view_t view>
constexpr click_type get_click_type()
{
    switch (view)
    {
        case game_view_t::menu_view:
        {
            return click_type::menu_view_click;
        }

        case game_view_t::universe_view:
        {
            if constexpr (std::is_same<size_settings::resource_area, AreaType>::value)
            {
                return click_type::universe_view_resource_click;
            }
            if constexpr (std::is_same<size_settings::play_area, AreaType>::value)
            {
                return click_type::universe_view_play_click;
            }
            if constexpr (std::is_same<size_settings::context_area, AreaType>::value)
            {
                return click_type::universe_view_context_click;
            }
            return click_type::universe_view_other_click;
        }

        case game_view_t::galaxy_view:
        {
            if constexpr (std::is_same<size_settings::resource_area, AreaType>::value)
            {
                return click_type::galaxy_view_resource_click;
            }
            if constexpr (std::is_same<size_settings::play_area, AreaType>::value)
            {
                return click_type::galaxy_view_play_click;
            }
            if constexpr (std::is_same<size_settings::context_area, AreaType>::value)
            {
                return click_type::galaxy_view_context_click;
            }
            return click_type::galaxy_view_other_click;
        }

        default:

            return click_type::other;
    }
}

constexpr bool is_implemented(click_type ct)
{
    return ct == click_type::menu_view_click || ct == click_type::universe_view_play_click ||
           ct == click_type::galaxy_view_resource_click || ct == click_type::universe_view_resource_click ||
           ct == click_type::galaxy_view_play_click;
}

struct game_gui
{
    // whenever we want to create a button, it should be done using this as id
    int user_button_id = 50;

    std::optional<std::shared_ptr<Sector>> current_sector;
    std::optional<std::shared_ptr<SectorObject>> current_object;
    std::optional<int> focused_button;

    std::vector<ui_button> popup_buttons;

    game_gui() {}

    int get_next_button_id() { return user_button_id++; }

    template <game_view_t view>
    void handleMouse(client_context& context, Uint32 event_type, SDL_MouseButtonEvent m, int x, int y);

    template <
        MouseButton Button, typename AreaType, game_view_t GameView,
        typename std::enable_if_t<get_click_type<AreaType, GameView>() == click_type::menu_view_click, void>* = nullptr>
    void click_handler(client_context& context, int x, int y, SDL_MouseButtonEvent m)
    {
        auto& gs = context.gs;

        x = x - size_settings::popup_menu_area::x_offset;
        y = y - size_settings::popup_menu_area::y_offset;

        for (const auto& b : popup_buttons)
        {
            if (sdl_utilities::check_collision(x, y, b.x, b.y, b.w, b.h))
            {
                // calling a lambda, possibly losing a few instructions
                b.click_handler(context);
            }
        }
    }

    template <MouseButton Button, typename AreaType, game_view_t GameView,
              typename std::enable_if_t<get_click_type<AreaType, GameView>() == click_type::galaxy_view_resource_click,
                                        void>* = nullptr>
    void click_handler(client_context& context, int x, int y, SDL_MouseButtonEvent m)
    {
        auto& gs = context.gs;
    }

    template <MouseButton Button, typename AreaType, game_view_t GameView,
              typename std::enable_if_t<
                  get_click_type<AreaType, GameView>() == click_type::universe_view_resource_click, void>* = nullptr>
    void click_handler(client_context& context, int x, int y, SDL_MouseButtonEvent m)
    {
        auto& gs = context.gs;
    }

    template <MouseButton Button, typename AreaType, game_view_t GameView,
              typename std::enable_if_t<get_click_type<AreaType, GameView>() == click_type::universe_view_play_click,
                                        void>* = nullptr>
    void click_handler(client_context& context, int x, int y, SDL_MouseButtonEvent m)
    {
        auto& gs = context.gs;
        auto& gr = context.gr;

        x = x - AreaType::x_offset;
        y = y - AreaType::y_offset;
        const int sprite_off = planets_meta::texture_size[SECTOR_1] * planets_meta::sector_multiplier * 0.5f;
        for (const auto s : gs.galaxy.value().sectors)
        {
            if (sdl_utilities::check_collision(
                    x, y, size_settings::play_area::width * (0.5f + 0.5f * s->position.x) - sprite_off,
                    size_settings::play_area::height * (0.5f + 0.5f * s->position.y) - sprite_off,
                    planets_meta::texture_size[SECTOR_1] * planets_meta::sector_multiplier,
                    planets_meta::texture_size[SECTOR_1] * planets_meta::sector_multiplier))
            {
                if (current_sector.has_value() && current_sector.value()->sector_id == s->sector_id)
                {
                    gs.set_view(game_view_t::galaxy_view);
                }
                current_sector = s;
                return;
            }
        }
        current_sector.reset();
    }

    template <MouseButton Button, typename AreaType, game_view_t GameView,
              typename std::enable_if_t<get_click_type<AreaType, GameView>() == click_type::galaxy_view_play_click,
                                        void>* = nullptr>
    void click_handler(client_context& context, int x, int y, SDL_MouseButtonEvent m)
    {
        x = x - AreaType::x_offset;
        y = y - AreaType::y_offset;

        auto& gs = context.gs;
        const auto& curr = gs.gui->current_sector.value();
        for (const auto& sec_obj : curr->objects)
        {
            const auto& pos = sec_obj->position;
            const auto tex_size = planets_meta::texture_size[GAS_GIANT_1];
            if (sdl_utilities::check_collision(
                    x, y, 
                    size_settings::play_area::width * (1.0f + pos.x) * 0.5f - 0.5 * tex_size,
                    size_settings::play_area::height * (1.0f + pos.y) * 0.5f - 0.5 * tex_size, 
                    tex_size, 
                    tex_size))
            {
                if (current_object.has_value() && current_object.value()->position == sec_obj->position)
                {
                    gs.set_view(game_view_t::planet_view);
                    gs.gui->current_object.reset();
                }
                current_object = sec_obj;
                return;
            }
        }
        current_object.reset();
    }

    template <MouseButton Button, typename AreaType, game_view_t GameView,
              typename std::enable_if_t<!is_implemented(get_click_type<AreaType, GameView>()), void>* = nullptr>
    void click_handler(client_context& context, int x, int y, SDL_MouseButtonEvent m)
    {
    }

    template <typename AreaType, game_view_t GameView>
    typename std::enable_if_t<std::is_same<AreaType, size_settings::popup_menu_area>::value, void> motion_handler(
        client_context& context, int x, int y)
    {
        x = x - AreaType::x_offset;
        y = y - AreaType::y_offset;
        for (auto& b : context.gs.gui->popup_buttons)
        {
            if (sdl_utilities::check_collision(x, y, b.x, b.y, b.w, b.h))
            {
                focused_button = b.button_id;
                return;
            }
        }
        focused_button.reset();
    }

    template <typename AreaType, game_view_t GameView>
    typename std::enable_if_t<!std::is_same<AreaType, size_settings::popup_menu_area>::value, void> motion_handler(
        client_context& context, int x, int y)
    {
        focused_button.reset();
    }
};

template <game_view_t view>
void game_gui::handleMouse(client_context& context, Uint32 event_type, SDL_MouseButtonEvent m, int x, int y)
{
    auto& gs = context.gs;

    switch (event_type)
    {
        case SDL_MOUSEMOTION:
        {
            if constexpr (view == game_view_t::menu_view)
            {
                if (sdl_utilities::check_view_area_collision<size_settings::popup_menu_area>(x, y))
                {
                    motion_handler<size_settings::popup_menu_area, view>(context, x, y);
                    return;
                }
            }
            if (sdl_utilities::check_view_area_collision<size_settings::play_area>(x, y))
            {
                motion_handler<size_settings::play_area, view>(context, x, y);
                return;
            }

            if (sdl_utilities::check_view_area_collision<size_settings::resource_area>(x, y))
            {
                motion_handler<size_settings::resource_area, view>(context, x, y);
                return;
            }

            if (sdl_utilities::check_view_area_collision<size_settings::context_area>(x, y))
            {
                motion_handler<size_settings::context_area, view>(context, x, y);
                return;
            }

            if (sdl_utilities::check_view_area_collision<size_settings::window_area>(x, y))
            {
                motion_handler<size_settings::window_area, view>(context, x, y);
                return;
            }
        }
        break;

        case SDL_MOUSEBUTTONDOWN:
        {
            if (sdl_utilities::check_view_area_collision<size_settings::play_area>(x, y))
            {
                m.button == SDL_BUTTON_LEFT
                    ? click_handler<MouseButton::MOUSE_LEFT, size_settings::play_area, view>(context, x, y, m)
                    : click_handler<MouseButton::MOUSE_RIGHT, size_settings::play_area, view>(context, x, y, m);
                break;
            }

            if (sdl_utilities::check_view_area_collision<size_settings::resource_area>(x, y))
            {
                m.button == SDL_BUTTON_LEFT
                    ? click_handler<MouseButton::MOUSE_LEFT, size_settings::resource_area, view>(context, x, y, m)
                    : click_handler<MouseButton::MOUSE_RIGHT, size_settings::resource_area, view>(context, x, y, m);
                break;
            }

            if (sdl_utilities::check_view_area_collision<size_settings::context_area>(x, y))
            {
                m.button == SDL_BUTTON_LEFT
                    ? click_handler<MouseButton::MOUSE_LEFT, size_settings::context_area, view>(context, x, y, m)
                    : click_handler<MouseButton::MOUSE_RIGHT, size_settings::context_area, view>(context, x, y, m);
                break;
            }

            if (sdl_utilities::check_view_area_collision<size_settings::window_area>(x, y))
            {
                m.button == SDL_BUTTON_LEFT
                    ? click_handler<MouseButton::MOUSE_LEFT, size_settings::window_area, view>(context, x, y, m)
                    : click_handler<MouseButton::MOUSE_RIGHT, size_settings::window_area, view>(context, x, y, m);
                break;
            }
        }
        break;
        default:
            break;
    }
}
#endif