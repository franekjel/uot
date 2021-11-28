#ifdef __linux__
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#elif _WIN32
#include "SDL.h"
#include "SDL_image.h"
#endif

#include "assets.h"
#include "game_rendering.h"
#include "resource_manager.h"
#include "game_gui.h"
#include "game_state.h"
#include "sdl_utilities.h"
#include "singleton.h"
#include "size_settings.h"
#include "client_context.h"
#include "input_handlers.h"

#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

// values will be set only once

void init(client_context& context)
{
    auto& gs = context.gs;
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        throw std::runtime_error("SDL coudl not initialize! SDL Error: %s\n" + std::string(SDL_GetError()));
    }

    context.w = 
        sdl_utilities::sdl_create_window("UOT Sketch", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                                   size_settings::window_area::width,
                                                   size_settings::window_area::height, SDL_WINDOW_SHOWN);

    context.r = 
        sdl_utilities::sdl_create_renderer(context.w, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags))
    {
        throw std::runtime_error("Couldn't initialize SDL_image");
    }

    gs.set_view(game_view_t::menu_view);
}

void loadMedia(client_context& context)
{
    auto& gr = context.gr;
    auto& gs = context.gs;

    printf("Loading background\n");
    gr.bk_texture =
        sdl_utilities::load_texture_from_file(std::string(basic_textures::background_texture_path), context.r);
    gr.buttonTextures.resize(buttons_meta::num_buttons);
    printf("Loading start button\n");
    gr.buttonTextures[button_types::START_BUTTON] = sdl_utilities::load_texture_from_file(
        std::string{basic_textures::menu_start_button_texture}, context.r);
    printf("Loading exit button\n");
    gr.buttonTextures[button_types::EXIT_BUTTON] =
        sdl_utilities::load_texture_from_file(std::string{basic_textures::menu_exit_button_texture}, context.r);

    printf("Loading universe button\n");
    gr.buttonTextures[button_types::UNIVERSE_BUTTON] =
        sdl_utilities::load_texture_from_file(std::string{basic_textures::menu_universe_button}, context.r);
    // load only the waiting screen planet texture
    gr.planetTextures.resize(planets_meta::num_planets);

    printf("Loading sector object buttons\n");
    gr.planetTextures[planet_types::TERRAN_START_PLANET] =
        sdl_utilities::load_texture_from_file(std::string{basic_textures::menu_planet_texture_path}, context.r);
    
    // load utility selection textures
    printf("Loading selection textures\n");
    gr.selectionTextures.resize(selection_meta::num_selection_textures);
    gr.selectionTextures[selection_types::SECTOR_SELECTION] =
        sdl_utilities::load_texture_from_file(std::string{basic_textures::sector_selection}, context.r);
}

void close()
{
    IMG_Quit();
    SDL_Quit();
}

int main(int argc, char* argv[])
{
    // no need for shared pointer, singleton is stack managed
    client_context context { singleton<resource_manager>::reference(), singleton<game_state_t>::reference() };

    init(context);
    context.gs.reset_galaxy();
    context.gs.set_gui();
    loadMedia(context);


    bool quit = false;
    SDL_Event e;
    while (!quit)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
            else if (e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP)
            {
                int x, y;
                SDL_GetMouseState(&x, &y);
                input_handlers::handleMouse(context, e.type, e.button, x, y);
            }
        }

        game_rendering::draw(context);
    }

    close();
    return 0;
}
