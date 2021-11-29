#ifndef CLIENT_CONTEXT_H
#define CLIENT_CONTEXT_H

#include "game_state.h"
#include "resource_manager.h"
#include "sdl_utilities.h"

struct client_context
{
    resource_manager& gr;
    game_state_t& gs;
    std::shared_ptr<SDL_Renderer> r;
    std::shared_ptr<SDL_Window> w;
};

#endif