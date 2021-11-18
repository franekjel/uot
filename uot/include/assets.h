#ifndef ASSETS_H
#define ASSETS_H

// Planetoid spritesheets generated with 
// https://deep-fold.itch.io/pixel-planet-generator


#include <array>
#include <string_view>

enum button_types
{
    START_BUTTON = 0,
    EXIT_BUTTON = 1,
    UNIVERSE_BUTTON = 2
};

enum selection_types 
{
    SECTOR_SELECTION = 0,
    SECTOR_OBJECT_SELECTION = 1,
    SHIP_SELECTION = 2
};

enum planet_types
{
    TERRAN_START_PLANET = 0
};

namespace selection_meta {
    constexpr int num_selection_textures = 2;
    constexpr int texture_width = 400;
    constexpr int texture_height = 400;
}

namespace generation_meta {
    constexpr bool use_uniform_sectors = false;
    constexpr int num_sectors = 16;
    constexpr int sqrt_num_sectors = 4;
    constexpr float multiplier = 5.0f;
}

namespace planets_meta
{
    constexpr int num_planets = 1;
    constexpr int num_frames = 50;
    constexpr int frame_width = 100;
    constexpr int frame_height = 100;
    constexpr int frame_duration = 4;
}  // namespace planets_meta

namespace basic_textures
{
    constexpr std::string_view background_texture_path = "assets/background_texture.png";
    constexpr std::string_view menu_planet_texture_path = "assets/menu_planet_sheet.png";
    constexpr std::string_view menu_start_button_texture = "assets/menu_start_button.png";
    constexpr std::string_view menu_exit_button_texture = "assets/menu_exit_button.png";
    constexpr std::string_view menu_universe_button = "assets/menu_universe_button.png";
    constexpr std::string_view sector_selection = "assets/sector_selection.png";
}  // namespace basic_textures

namespace buttons_meta
{
    constexpr int num_buttons = 3;
    constexpr int button_texture_height = 100;
    constexpr int button_texture_width = 300;
    constexpr int button_height = 75;
    constexpr int button_width = 200;
    constexpr int button_x_offset = 25;
    constexpr int button_y_offset = 110;

    static constexpr std::array<int, 3> all_button = {START_BUTTON, EXIT_BUTTON, UNIVERSE_BUTTON};

    static constexpr std::array<int, 2> menu_buttons = {START_BUTTON, EXIT_BUTTON};

    static constexpr std::array<int, 1> universe_buttons = {EXIT_BUTTON};

    static constexpr std::array<int, 2> galaxy_buttons = {UNIVERSE_BUTTON, EXIT_BUTTON};

}  // namespace buttons_meta

#endif
