#include <input_handlers.h>
#include "game_gui.h"

// one level higher than GUI input handling, maybe in the future we won't want the GUI to handle everything
// (separate classes for main menu or whatever )

void input_handlers::handleMouse(client_context& context, Uint32 event_type, SDL_MouseButtonEvent m, int x, int y)
{
    auto& gui = context.gs.gui;
    auto& gs = context.gs;

    // check if menu area
    switch (gs.get_view())
    {
        case game_view_t::menu_view:
            gui->template handleMouse<game_view_t::menu_view>(context, event_type, m, x, y);
            break;
        case game_view_t::galaxy_view:
            gui->template handleMouse<game_view_t::galaxy_view>(context, event_type, m, x, y);
            break;
        case game_view_t::universe_view:
            gui->template handleMouse<game_view_t::universe_view>(context, event_type, m, x, y);
            break;
        default:
            break;
    }
}

void input_handlers::handle_keydown(client_context& context, Uint16 k)
{
    auto& gui = context.gs.gui;
    auto& gs = context.gs;

    if (k == SDLK_ESCAPE)
    {
        switch (gs.get_view())
        {
            case game_view_t::menu_view:
                // TODO add exiting the game with popup
                break;
            case game_view_t::galaxy_view:
                // TODO add popup asking whether you want to finish the game
                gs.gui->current_object.reset();
                gs.set_view(game_view_t::universe_view);
                break;
            case game_view_t::universe_view:
                gs.gui->current_sector.reset();
                gs.set_view(game_view_t::universe_view);
                gs.set_view(game_view_t::menu_view);
                break;
            case game_view_t::planet_view:
                gs.set_view(game_view_t::galaxy_view);
                break;

            default:
                break;
        }
    }
}
