#ifdef __linux__
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#elif _WIN32
#include "SDL.h"
#include "SDL_image.h"
#endif

#include "assets.h"
#include "game_gui.h"
#include "game_resources.h"
#include "game_state.h"
#include "rendering.h"

#include "rendering_all_views.h"

#include "sdl_utilities.h"
#include "setup_utils.h"
#include "singleton.h"
#include "size_settings.h"

#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace su = setup_utils;

void close(client_context& context)
{
    // Free global font

    // RAII (custom shared_ptr deleters) used everywhere so no need to close
    // textures and/or fonts manually

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

int main(int argc, char* argv[])
{
    client_context context{singleton<game_resources>::reference(), singleton<game_state>::reference()};

    su::init(context);
    su::loadMedia(context);

    volatile bool quit = false;
    SDL_Event e;
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

    close(context);
    return 0;
}
