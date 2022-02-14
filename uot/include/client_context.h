#ifndef CLIENT_CONTEXT_H
#define CLIENT_CONTEXT_H

#include <mutex>
#include <variant>

#include "rendering.h"
#include "rendering_all_views.h"
#include "rendering_views.h"
#include "sdl_utilities.h"

struct game_gui;
struct game_resources;
struct game_state;
struct uot_net_client;
struct msg_queue;

template <typename T>
struct LockGuardedValue
{
    T& value;
    LockGuardedValue(T& value, std::recursive_mutex& mutex) : value(value), lock(mutex) {}

   private:
    std::unique_lock<std::recursive_mutex> lock;
};

struct client_context
{
    std::shared_ptr<game_resources> gr;
    rendering::view_t view;
    std::shared_ptr<SDL_Renderer> r;
    std::shared_ptr<SDL_Window> w;
    std::shared_ptr<game_gui> gui;

    unsigned int player_id;

    client_context(const std::shared_ptr<game_resources> gr, const std::shared_ptr<game_state> gs,
                   std::shared_ptr<msg_queue> mq)
        : gr(gr), gs(gs), mq(mq){};

    LockGuardedValue<std::shared_ptr<game_state>> getGameState()
    {
        return LockGuardedValue<std::shared_ptr<game_state>>(gs, gs_mutex);
    }

    LockGuardedValue<std::shared_ptr<msg_queue>> getActionQueue()
    {
        return LockGuardedValue<std::shared_ptr<msg_queue>>(mq, mq_mutex);
    }

    ~client_context()
    {
        Mix_CloseAudio();
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
    }

    std::mutex fleet_mutex;

   private:
    std::recursive_mutex gs_mutex;
    std::shared_ptr<game_state> gs;

    std::recursive_mutex mq_mutex;
    std::shared_ptr<msg_queue> mq;
};

#endif
