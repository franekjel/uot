#include "setup_utils.h"
#include "assets.h"
// all necessary headers come through game_gui
#include "SDL2/SDL_mixer.h"
#include "game_gui.h"
#include "game_resources.h"
#include "game_state.h"
#include "gui/buttons.h"

namespace setup_utils
{
void init(client_context& context)
{
    auto state = context.getGameState();
    auto& gs = state.value;

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
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

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
    {
        throw std::runtime_error("Couldn't initialize SDL_mixer");
    }

    context.gui = std::make_unique<game_gui>();
    context.gui->popup_buttons.push_back(std::make_unique<start_button>());
    context.gui->popup_buttons.push_back(std::make_unique<exit_button>());
    context.gui->navigation_menu_buttons.push_back(std::make_unique<technology_button>());
}

void loadMedia(client_context& context)
{
    auto& gr = context.gr;

    printf("Loading background\n");
    gr->bk_texture =
        sdl_utilities::load_texture_from_file(std::string(basic_textures::background_texture_path), context.r);
    printf("Loading sky\n");
    gr->sky_texture = sdl_utilities::load_texture_from_file(std::string(basic_textures::sky_texture_path), context.r);
    gr->sky_square_texture =
        sdl_utilities::load_texture_from_file(std::string(basic_textures::sky_square_texture_path), context.r);

    printf("Loading buildings\n");
    gr->buildings_sprite =
        sdl_utilities::load_texture_from_file(std::string(basic_textures::building_sprite_path), context.r);
    gr->buildings_blur_sprite =
        sdl_utilities::load_texture_from_file(std::string(basic_textures::building_blur_sprite_path), context.r);

    printf("Loading resources\n");
    gr->resource_texture =
        sdl_utilities::load_texture_from_file(std::string(resources_meta::resources_path), context.r);

    {
        const int r = (size_settings::play_area::height / 2 - 2);
        const std::string svg = "<svg height='" + std::to_string(r * 2) + "' width='" + std::to_string(r * 2) +
                                "'><circle cx='" + std::to_string(r) + "' cy='" + std::to_string(r) + "' r='" +
                                std::to_string(r) +
                                "' stroke='#eeeeee' stroke-width='3' "
                                "fill='none' /></svg>";
        gr->sector_boundary = texture_t{r * 2, r * 2, sdl_utilities::load_texture_from_svg(svg, context.r)};
    }

    {
        const int r = std::sqrt(Fleet::kNearValue) * (size_settings::play_area::height / 2);
        const std::string svg = "<svg height='" + std::to_string(r * 2) + "' width='" + std::to_string(r * 2) +
                                "'><circle cx='" + std::to_string(r) + "' cy='" + std::to_string(r) + "' r='" +
                                std::to_string(r) +
                                "' stroke='#00ffff' stroke-width='2' "
                                "fill='none' /></svg>";
        gr->jump_zone = texture_t{r * 2, r * 2, sdl_utilities::load_texture_from_svg(svg, context.r)};
    }

    printf("Loading  portal texture");
    // load only the waiting screen planet texture
    gr->planetTextures.resize(planets_meta::num_planets);

    printf("Loading sector object textures\n");
    for (int i = 0; i < planets_meta::num_planets; ++i)
    {
        gr->planetTextures[i] = texture_t{
            planets_meta::texture_size[i], planets_meta::texture_size[i],
            sdl_utilities::load_texture_from_file(std::string{planets_meta::planet_texture_paths[i]}, context.r)};
    }

    printf("Loading fleet texture\n");
    for (int i = 0; i < gr->fleet_textures.size(); i++)
    {
        gr->fleet_textures[i] =
            texture_t{32, 32,
                      sdl_utilities::load_and_paint_texture_from_file(std::string(basic_textures::fleet_sprite_path),
                                                                      context.r, colors_meta::user_color[i])};
    }

    for (const auto& [t, m] : Modules)
    {
        if (!m.weapon.has_value())
            continue;
        const int r = m.weapon.value().range * (size_settings::play_area::height / 2);
        const std::string svg = "<svg height='" + std::to_string(r * 2) + "' width='" + std::to_string(r * 2) +
                                "'><circle cx='" + std::to_string(r) + "' cy='" + std::to_string(r) + "' r='" +
                                std::to_string(r) +
                                "' stroke='white' stroke-width='1' "
                                "fill='none' /></svg>";
        SDL_RWops* rw = SDL_RWFromConstMem(svg.c_str(), svg.size());
        SDL_Surface* surface = IMG_Load_RW(rw, 1);
        gr->circle_textures[t] = texture_t{r * 2, r * 2, sdl_utilities::load_texture_from_svg(svg, context.r)};
        SDL_FreeSurface(surface);
    }

    // load utility selection textures
    printf("Loading selection textures\n");
    gr->selectionTextures.resize(selection_meta::num_selection_textures);
    gr->selectionTextures[selection_types::SECTOR_SELECTION] =
        sdl_utilities::load_texture_from_file(std::string{basic_textures::sector_selection}, context.r);

    // FONTS
    printf("Loading the main font\n");
    gr->main_font = sdl_utilities::load_font(std::string(fonts::main_font), fonts::main_font_size);

    printf("Loading the secondary font\n");
    gr->secondary_font = sdl_utilities::load_font(std::string(fonts::secondary_font), fonts::secondary_font_size);

    printf("Loading the resources font\n");
    gr->resource_font = sdl_utilities::load_font(std::string(fonts::secondary_font), fonts::resource_font_size);

    printf("Loading the action button font font\n");
    gr->infobox_font = sdl_utilities::load_font(std::string(fonts::secondary_font), fonts::action_button_font_size);

    printf("Loading the infobox font\n");
    gr->action_button_font = sdl_utilities::load_font(std::string(fonts::secondary_font), fonts::infobox_font_size);

    // AUDIO

    // Load the music
    gr->ambient = sdl_utilities::load_music(std::string(audio_meta::ambient_filename));

    // Load chunks
    gr->click = sdl_utilities::load_chunk(std::string(audio_meta::click_filename));
    gr->open_planet = sdl_utilities::load_chunk(std::string(audio_meta::open_planet_filename));
    gr->open_ship = sdl_utilities::load_chunk(std::string(audio_meta::open_ship_filename));
    gr->scanning = sdl_utilities::load_chunk(std::string(audio_meta::scanning_filename));
}
}  // namespace setup_utils
