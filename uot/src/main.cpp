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
    client_context context(singleton<game_resources>::ptr(), singleton<game_state>::ptr());
    {
        uot_net_client nc(context);

        su::init(context);
        su::loadMedia(context);

        // Play the music
        if (Mix_PlayMusic(context.gr->ambient.get(), -1) == -1)
        {
            throw std::runtime_error("failed to play the music track");
        }

        volatile bool quit = false;
        SDL_Event e;

        nc.connect_to_server();
        while (!quit)
        {
            while (SDL_PollEvent(&e) != 0)
            {
                if (e.type == SDL_QUIT)
                {
                    quit = true;
                }
                else if (e.type == SDL_KEYDOWN)
                {
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
        }

        nc.disconnect_from_server();
    }

    // client_context destructor closes SDL2 libs

    return 0;
}
