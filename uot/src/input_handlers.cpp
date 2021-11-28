#include <input_handlers.h>
#include "game_gui.h"

// one level higher than GUI input handling, maybe in the future we won't want the GUI to handle everything
// (separate classes for main menu or whatever )

void input_handlers::handleMouse(client_context& context, Uint32 event_type, SDL_MouseButtonEvent m, int x, int y) {
    auto& gui = context.gs.gui;

    if (sdl_utilities::check_view_area_collision<size_settings::play_area>(x, y)) {
        gui->template handleMouse<size_settings::play_area>(context, event_type, m, x, y);
        return;
    }

    if (sdl_utilities::check_view_area_collision<size_settings::button_area>(x, y)) {
        gui->template handleMouse<size_settings::button_area>(context, event_type, m, x, y);
        return;
    }

    if (sdl_utilities::check_view_area_collision<size_settings::context_area>(x, y)) {
        gui->template handleMouse<size_settings::context_area>(context, event_type, m, x, y);
        return;
    }

    if (sdl_utilities::check_view_area_collision<size_settings::window_area>(x, y)) {
        gui->template handleMouse<size_settings::window_area>(context, event_type, m, x, y);
        return;
    }
}
