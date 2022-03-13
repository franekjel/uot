#ifdef __linux__
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_mixer.h"
#elif _WIN32
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#endif

#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include "assets.h"
#include "game_gui.h"
#include "game_resources.h"
#include "game_state.h"
#include "msg_queue.h"
#include "net/uot_net_client.h"
#include "rendering.h"
#include "rendering_all_views.h"
#include "sdl_utilities.h"
#include "setup_utils.h"
#include "singleton.h"
#include "size_settings.h"

namespace su = setup_utils;

void close(client_context& context)
{
    // Free global font

    // RAII (custom shared_ptr deleters) used everywhere so no need to close
    // textures and/or fonts manually
}

int main(int argc, char* argv[])
{
    client_context context(singleton<game_resources>::ptr(), singleton<game_state>::ptr(), singleton<msg_queue>::ptr());
    {
        uot_net_client nc(context);

        su::init(context);
        su::loadMedia(context);

        // Play the music
        //if (Mix_PlayMusic(context.gr->ambient.get(), -1) == -1)
        //{
        //    throw std::runtime_error("failed to play the music track");
        //}

        volatile bool quit = false;
        SDL_Event e;

#ifdef UOT_DEBUG
#define FPS_INTERVAL 1.0                       // seconds.
        Uint32 fps_lasttime = SDL_GetTicks();  // the last recorded time.
        Uint32 fps_current;                    // the current FPS.
        Uint32 fps_frames = 0;                 // frames passed since the last recorded fps.
#endif
        if (argc > 1)
        {
            std::string ipv4(argv[1]);
            nc.connect_to_server(ipv4);
        }
        else
            nc.connect_to_server();

        while (!quit)
        {
            {
                auto& player = context.getGameState().value->player;
                if (player && (player->is_winner || player->is_loser))
                    context.view = std::make_shared<rendering::render_end_view>();
            }

            while (SDL_PollEvent(&e) != 0)
            {
                if (e.type == SDL_QUIT)
                {
                    quit = true;
                }
                else if (e.type == SDL_KEYDOWN)
                {
                    if (e.key.keysym.sym == SDLK_o && e.key.keysym.mod & KMOD_CTRL)
                        context.getGameState().value->player->is_winner = true;
                    else if (e.key.keysym.sym == SDLK_l && e.key.keysym.mod & KMOD_CTRL)
                        context.getGameState().value->player->is_loser = true;

                    std::visit([&](auto&& v) { v->key_handler(context, e.key.keysym.sym); }, context.view);
                }
                else if (e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP)
                {
                    int x, y;
                    SDL_GetMouseState(&x, &y);
                    std::visit([&](auto&& v) { v->mouse_handler(context, e.type, e.button, x, y); }, context.view);
                }
                else if (e.type == SDL_MOUSEWHEEL)
                {
                    int x, y;
                    int xmov{e.wheel.x}, ymov{e.wheel.y};
                    SDL_GetMouseState(&x, &y);
                    std::visit([&](auto&& v) { v->wheel_handler(context, x, y, xmov, ymov); }, context.view);
                }
            }

            // static polymorphism via CRTP
            // and std::variant
            std::visit([&](auto&& v) { v->draw(context); }, context.view);

            // these shouldn't be here
            // put into some EndFrame() in sdl_utilities
            SDL_RenderPresent(context.r.get());

            context.gui->planet_frame =
                (context.gui->planet_frame + 1 == (planets_meta::num_frames * planets_meta::frame_duration))
                    ? 0
                    : context.gui->planet_frame + 1;

#ifdef UOT_DEBUG
            fps_frames++;
            if (fps_lasttime < SDL_GetTicks() - FPS_INTERVAL * 1000)
            {
                fps_lasttime = SDL_GetTicks();
                fps_current = fps_frames;
                std::cout << "\n\n FPS:  " << fps_current << "\n\n";
                fps_frames = 0;
            }
#endif
        }

        nc.disconnect_from_server();
    }

    // client_context destructor closes SDL2 libs

    return 0;
}
