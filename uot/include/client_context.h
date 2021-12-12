#ifndef CLIENT_CONTEXT_H
#define CLIENT_CONTEXT_H

#include <variant>
#include "game_gui.h"
#include "game_resources.h"
#include "game_state.h"
#include "rendering.h"
#include "rendering_all_views.h"
#include "rendering_views.h"
#include "sdl_utilities.h"

struct client_context
{
    game_resources& gr;
    game_state& gs;
    rendering::view_t view;
    std::shared_ptr<SDL_Renderer> r;
    std::shared_ptr<SDL_Window> w;
    std::shared_ptr<game_gui> gui;
};

#endif
