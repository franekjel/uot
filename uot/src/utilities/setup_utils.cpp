#include "setup_utils.h"
#include "assets.h"
// all necessary headers come through game_gui
#include "game_gui.h"
#include "ui_menu_lists.h"

namespace setup_utils
{
void init(client_context& context)
{
    auto& gs = context.gs;
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        throw std::runtime_error("SDL coudl not initialize! SDL Error: %s\n" + std::string(SDL_GetError()));
    }

    context.w = sdl_utilities::sdl_create_window("UOT Sketch", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                                 size_settings::window_area::width, size_settings::window_area::height,
                                                 SDL_WINDOW_SHOWN);

    context.r = sdl_utilities::sdl_create_renderer(context.w, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags))
    {
        throw std::runtime_error("Couldn't initialize SDL_image");
    }

    if (TTF_Init() == -1)
    {
        throw std::runtime_error("Couldn't initialize SDL_ttf");
    }

    context.gs->reset_galaxy();
    context.gui = std::make_unique<game_gui>();
    context.gui->popup_buttons =
        std::vector(std::begin(ui_menu_lists::start_menu), std::end(ui_menu_lists::start_menu));
}

void loadMedia(client_context& context)
{
    auto& gr = context.gr;
    auto& gs = context.gs;

    printf("Loading background\n");
    gr->bk_texture =
        sdl_utilities::load_texture_from_file(std::string(basic_textures::background_texture_path), context.r);
    gr->sky_texture = sdl_utilities::load_texture_from_file(std::string(basic_textures::sky_texture_path), context.r);
    gr->resource_texture =
        sdl_utilities::load_texture_from_file(std::string(resources_meta::resources_path), context.r);
    gr->buttonTextures.resize(buttons_meta::num_buttons);
    printf("Loading start button\n");
    gr->buttonTextures[button_types::START_BUTTON] =
        sdl_utilities::load_texture_from_file(std::string{basic_textures::menu_start_button_texture}, context.r);
    printf("Loading exit button\n");
    gr->buttonTextures[button_types::EXIT_BUTTON] =
        sdl_utilities::load_texture_from_file(std::string{basic_textures::menu_exit_button_texture}, context.r);

    printf("Loading universe button\n");
    gr->buttonTextures[button_types::UNIVERSE_BUTTON] =
        sdl_utilities::load_texture_from_file(std::string{basic_textures::menu_universe_button}, context.r);
    // load only the waiting screen planet texture
    gr->planetTextures.resize(planets_meta::num_planets);

    printf("Loading sector object textures\n");
    for (int i = 0; i < planets_meta::num_planets; ++i)
    {
        gr->planetTextures[i] = texture_t{
            planets_meta::texture_size[i], planets_meta::texture_size[i],
            sdl_utilities::load_texture_from_file(std::string{planets_meta::planet_texture_paths[i]}, context.r)};
    }

    // load utility selection textures
    printf("Loading selection textures\n");
    gr->selectionTextures.resize(selection_meta::num_selection_textures);
    gr->selectionTextures[selection_types::SECTOR_SELECTION] =
        sdl_utilities::load_texture_from_file(std::string{basic_textures::sector_selection}, context.r);

    // FONTS
    gr->main_font = sdl_utilities::load_font(std::string(fonts::main_font), fonts::main_font_size);

    gr->secondary_font = sdl_utilities::load_font(std::string(fonts::secondary_font), 25);

    gr->resource_font = sdl_utilities::load_font(std::string(fonts::secondary_font), fonts::resource_font_size);
}
}  // namespace setup_utils
