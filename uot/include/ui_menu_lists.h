#ifndef UI_MENU_LISTS_H
#define UI_MENU_LISTS_H
#include <array>
#include "ui_button.h"
#include "assets.h"
#include "client_context.h"
#include "sdl_utilities.h"

// for menus that are always the same

namespace ui_menu_lists {

    const std::vector<ui_button> start_menu = { 
        ui_button { 
            0,
            (size_settings::popup_menu_area::width - buttons_meta::button_width ) / 2,
            planets_meta::frame_height / 2+ 60 + buttons_meta::button_y_offset, 
            buttons_meta::button_width, 
            buttons_meta::button_height, 
            START_BUTTON, 
            [] (client_context& context) -> void {
               context.gs.set_view(game_view_t::universe_view);
            }},
        ui_button { 
            1,
            (size_settings::popup_menu_area::width - buttons_meta::button_width) / 2,
            planets_meta::frame_height / 2+ 60 + 2 * buttons_meta::button_y_offset, 
            buttons_meta::button_width, 
            buttons_meta::button_height, 
            EXIT_BUTTON, 
            [] (client_context& context) -> void {
               SDL_Event sdlevent;
               sdlevent.type = SDL_QUIT;
               SDL_PushEvent(&sdlevent);
            }}
    };
}

#endif