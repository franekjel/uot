#ifndef GAME_GUI_H
#define GAME_GUI_H

#include <sector.h>
#include <sectorobject.h>
#include <iostream>
#include "assets.h"
#include "client_context.h"
#include "game_state.h"

enum class MouseButton
{
    MOUSE_LEFT,
    MOUSE_RIGHT
};

enum class click_type
{
    menu_view_menu_click,
    menu_view_play_click,
    menu_view_context_click,
    menu_view_other_click,
    universe_view_menu_click,
    universe_view_play_click,
    universe_view_context_click,
    universe_view_other_click,
    galaxy_view_menu_click,
    galaxy_view_play_click,
    galaxy_view_context_click,
    galaxy_view_other_click,
    other
};

template <typename AreaType, game_view_t view>
constexpr click_type get_click_type()
{
    switch (view)
    {
        case game_view_t::menu_view:
        {
            if constexpr (std::is_same<size_settings::button_area, AreaType>::value)
            {
                return click_type::menu_view_menu_click;
            }
            if constexpr (std::is_same<size_settings::play_area, AreaType>::value)
            {
                return click_type::menu_view_play_click;
            }
            if constexpr (std::is_same<size_settings::context_area, AreaType>::value)
            {
                return click_type::menu_view_context_click;
            }
            return click_type::menu_view_other_click;
        }

        case game_view_t::universe_view:
        {
            if constexpr (std::is_same<size_settings::button_area, AreaType>::value)
            {
                return click_type::universe_view_menu_click;
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
            if constexpr (std::is_same<size_settings::button_area, AreaType>::value)
            {
                return click_type::galaxy_view_menu_click;
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
    return ct == click_type::menu_view_menu_click || ct == click_type::universe_view_play_click ||
           ct == click_type::galaxy_view_menu_click || ct == click_type::universe_view_menu_click ||
           ct == click_type::galaxy_view_play_click;
}

struct game_gui
{
    std::optional<std::shared_ptr<Sector>> current_sector;
    std::optional<std::shared_ptr<SectorObject>> current_object;
    std::optional<int> focused_button;

    game_gui() {}

    template <typename AreaType>
    void handleMouse(client_context& context, Uint32 event_type, SDL_MouseButtonEvent m, int x, int y);

    template <MouseButton Button, typename AreaType, game_view_t GameView,
              typename std::enable_if_t<get_click_type<AreaType, GameView>() == click_type::menu_view_menu_click,
                                        void>* = nullptr>
    void click_handler(client_context& context, int x, int y, SDL_MouseButtonEvent m)
    {
        auto& gs = context.gs;
        const auto clicked_button = buttons_meta::menu_buttons[y / buttons_meta::button_y_offset];
        if (clicked_button == button_types::START_BUTTON)
        {
            gs.set_view(game_view_t::universe_view);
            return;
        }

        if (clicked_button == button_types::EXIT_BUTTON)
        {
            SDL_Event sdlevent;
            sdlevent.type = SDL_QUIT;
            SDL_PushEvent(&sdlevent);
        }
    }

    template <MouseButton Button, typename AreaType, game_view_t GameView,
              typename std::enable_if_t<get_click_type<AreaType, GameView>() == click_type::galaxy_view_menu_click,
                                        void>* = nullptr>
    void click_handler(client_context& context, int x, int y, SDL_MouseButtonEvent m)
    {
        auto& gs = context.gs;
        const auto clicked_button = buttons_meta::galaxy_buttons[y / buttons_meta::button_y_offset];
        if (clicked_button == button_types::UNIVERSE_BUTTON)
        {
            gs.set_view(game_view_t::universe_view);
            gs.gui->current_object.reset();
            gs.gui->current_sector.reset();
            return;
        }

        if (clicked_button == button_types::EXIT_BUTTON)
        {
            SDL_Event sdlevent;
            sdlevent.type = SDL_QUIT;
            SDL_PushEvent(&sdlevent);
        }
    }

    template <MouseButton Button, typename AreaType, game_view_t GameView,
              typename std::enable_if_t<get_click_type<AreaType, GameView>() == click_type::universe_view_menu_click,
                                        void>* = nullptr>
    void click_handler(client_context& context, int x, int y, SDL_MouseButtonEvent m)
    {
        auto& gs = context.gs;
        const auto clicked_button = buttons_meta::universe_buttons[y / buttons_meta::button_y_offset];
        if (clicked_button == button_types::EXIT_BUTTON)
        {
            SDL_Event sdlevent;
            sdlevent.type = SDL_QUIT;
            SDL_PushEvent(&sdlevent);
        }
    }

    template <MouseButton Button, typename AreaType, game_view_t GameView,
              typename std::enable_if_t<get_click_type<AreaType, GameView>() == click_type::universe_view_play_click,
                                        void>* = nullptr>
    void click_handler(client_context& context, int x, int y, SDL_MouseButtonEvent m)
    {
        auto& gs = context.gs;
        for (const auto s : gs.galaxy.value().sectors)
        {
            if (sdl_utilities::check_sector_collision(
                    x, y, size_settings::play_area::width * (0.8f + 0.8f * s->position.x) / 2,
                    size_settings::play_area::height * (0.8f + 0.8f * s->position.y) / 2,
                    size_settings::play_area::width / generation_meta::sqrt_num_sectors,
                    size_settings::play_area::height / generation_meta::sqrt_num_sectors))
            {
                if (current_sector.has_value() && current_sector.value()->sector_id == s->sector_id)
                {
                    gs.set_view(game_view_t::galaxy_view);
                }
                current_sector = s;
                break;
            }
        }
    }

    template <MouseButton Button, typename AreaType, game_view_t GameView,
              typename std::enable_if_t<get_click_type<AreaType, GameView>() == click_type::galaxy_view_play_click,
                                        void>* = nullptr>
    void click_handler(client_context& context, int x, int y, SDL_MouseButtonEvent m)
    {
        auto& gs = context.gs;
        const auto& curr = gs.gui->current_sector.value();
        for (const auto& sec_obj : curr->objects)
        {
            const auto& pos = sec_obj->position;
            if (sdl_utilities::check_sector_collision(
                    x, y, size_settings::play_area::width * (1.0f + pos.x) * 0.5f - 0.5 * planets_meta::frame_width,
                    size_settings::play_area::height * (1.0f + pos.y) * 0.5f - 0.5 * planets_meta::frame_height,
                    planets_meta::frame_width, planets_meta::frame_height))
            {
                if (current_object.has_value() && current_object.value()->position == sec_obj->position)
                {
                    gs.set_view(game_view_t::planet_view);
                }
                current_object = sec_obj;
                break;
            }
        }
    }

    template <MouseButton Button, typename AreaType, game_view_t GameView,
              typename std::enable_if_t<!is_implemented(get_click_type<AreaType, GameView>()), void>* = nullptr>
    void click_handler(client_context& context, int x, int y, SDL_MouseButtonEvent m)
    {
    }

    template <typename AreaType, game_view_t GameView>
    typename std::enable_if_t<std::is_same<AreaType, size_settings::button_area>::value, void> motion_handler(
        client_context& context, int x, int y)
    {
        focused_button = y / buttons_meta::button_y_offset;
    }

    template <typename AreaType, game_view_t GameView>
    typename std::enable_if_t<!std::is_same<AreaType, size_settings::button_area>::value, void> motion_handler(
        client_context& context, int x, int y)
    {
        focused_button.reset();
    }
};

template <typename AreaType>
void game_gui::handleMouse(client_context& context, Uint32 event_type, SDL_MouseButtonEvent m, int x, int y)
{
    const auto _x = x - AreaType::x_offset;
    const auto _y = y - AreaType::y_offset;

    auto& gs = context.gs;

    switch (event_type)
    {
        case SDL_MOUSEMOTION:
        {
            // check if menu area
            switch (gs.game_view)
            {
                case game_view_t::menu_view:
                    motion_handler<AreaType, game_view_t::menu_view>(context, _x, _y);
                    break;
                case game_view_t::galaxy_view:
                    motion_handler<AreaType, game_view_t::galaxy_view>(context, _x, _y);
                    break;
                case game_view_t::universe_view:
                    motion_handler<AreaType, game_view_t::universe_view>(context, _x, _y);
                    break;
                default:
                    break;
            }
        }
        break;

        case SDL_MOUSEBUTTONDOWN:
        {
            switch (gs.game_view)
            {
                case game_view_t::menu_view:
                    m.button == SDL_BUTTON_LEFT
                        ? click_handler<MouseButton::MOUSE_LEFT, AreaType, game_view_t::menu_view>(context, _x, _y, m)
                        : click_handler<MouseButton::MOUSE_RIGHT, AreaType, game_view_t::menu_view>(context, _x, _y, m);
                    break;
                case game_view_t::galaxy_view:
                    m.button == SDL_BUTTON_LEFT
                        ? click_handler<MouseButton::MOUSE_LEFT, AreaType, game_view_t::galaxy_view>(context, _x, _y, m)
                        : click_handler<MouseButton::MOUSE_RIGHT, AreaType, game_view_t::galaxy_view>(context, _x, _y,
                                                                                                      m);
                    break;
                case game_view_t::universe_view:
                    m.button == SDL_BUTTON_LEFT
                        ? click_handler<MouseButton::MOUSE_LEFT, AreaType, game_view_t::universe_view>(context, _x, _y,
                                                                                                       m)
                        : click_handler<MouseButton::MOUSE_RIGHT, AreaType, game_view_t::universe_view>(context, _x, _y,
                                                                                                        m);
                    break;
                default:
                    break;
            }
        }
        break;
        default:
            break;
    }
}
#endif