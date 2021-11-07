#ifndef ASSETS_H
#define ASSETS_H

#include <string_view>

enum button_types
{
    START_BUTTON = 0,
    EXIT_BUTTON = 1,
    GALAXY_BUTTON = 2
};

enum planet_types
{
    TERRAN_START_PLANET = 0
};

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

static const int all_button[] = {START_BUTTON, EXIT_BUTTON, GALAXY_BUTTON};

static const int menu_buttons[] = {START_BUTTON, EXIT_BUTTON};

}  // namespace buttons_meta

#endif