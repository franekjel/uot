#ifndef CLIENT_CONTEXT_H
#define CLIENT_CONTEXT_H

#include <mutex>
#include <variant>

#include "game_gui.h"
#include "game_resources.h"
#include "game_state.h"
#include "rendering.h"
#include "rendering_all_views.h"
#include "rendering_views.h"
#include "sdl_utilities.h"

template <typename T>
struct LockGuardedValue
{
    T& value;
    LockGuardedValue(T& value, std::mutex& mutex) : value(value), lock(mutex) {}

   private:
    std::unique_lock<std::mutex> lock;
};

struct client_context
{
    std::shared_ptr<game_resources> gr;
    rendering::view_t view;
    std::shared_ptr<SDL_Renderer> r;
    std::shared_ptr<SDL_Window> w;
    std::shared_ptr<game_gui> gui;

    client_context(const std::shared_ptr<game_resources> gr, const std::shared_ptr<game_state> gs) : gr(gr), gs(gs){};
    LockGuardedValue<std::shared_ptr<game_state>> getGameState()
    {
        return LockGuardedValue<std::shared_ptr<game_state>>(gs, gs_mutex);
    }

   private:
    std::mutex gs_mutex;
    std::shared_ptr<game_state> gs;
};

#endif
