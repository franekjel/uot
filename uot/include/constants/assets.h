#ifndef ASSETS_H
#define ASSETS_H

// Planetoid spritesheets generated with
// https://deep-fold.itch.io/pixel-planet-generator

#include <array>
#include <string_view>

enum selection_types
{
    SECTOR_SELECTION = 0,
    SECTOR_OBJECT_SELECTION = 1,
    SHIP_SELECTION = 2
};

enum planet_types
{
    MENU_PLANET,
    SECTOR_1,
    SECTOR_2,
    SECTOR_3,
    SECTOR_4,
    SECTOR_5,
    SECTOR_6,
    SECTOR_7,
    SECTOR_8,
    SECTOR_9,
    SECTOR_10,
    GAS_GIANT_1,
    GAS_GIANT_2,
    GAS_GIANT_3,
    ISLANDS_1,
    ISLANDS_2,
    ISLANDS_3,
    NO_ATM_1,
    NO_ATM_2,
    NO_ATM_3,
    RINGS_1,
    RINGS_2,
    RINGS_3,
    TERRAN_D_1,
    TERRAN_D_2,
    TERRAN_D_3,
    TERRAN_W_1,
    TERRAN_W_2,
    TERRAN_W_3  // 29th
};

namespace selection_meta
{
constexpr int num_selection_textures = 2;
constexpr int texture_width = 400;
constexpr int texture_height = 400;
}  // namespace selection_meta

namespace generation_meta
{
constexpr bool use_uniform_sectors = false;
constexpr int num_sectors = 64;
constexpr int sqrt_num_sectors = 8;
constexpr float multiplier = 5.0f;
}  // namespace generation_meta

namespace planets_meta
{
constexpr float sector_multiplier = 0.3f;
constexpr int num_planets = 29;
constexpr int num_frames = 50;
constexpr int frame_width = 300;
constexpr int frame_height = 300;
constexpr int frame_duration = 4;

constexpr std::array<int, num_planets> texture_size = {300, 200, 200, 200, 200, 200, 200, 200, 200, 200,
                                                       200, 100, 100, 100, 100, 100, 100, 100, 100, 100,
                                                       300, 300, 300, 100, 100, 100, 100, 100, 100};

constexpr std::array<std::string_view, num_planets> planet_texture_paths = {
    "assets/menu_planet_sheet.png", "assets/sector_1.png",    "assets/sector_2.png",   "assets/sector_3.png",
    "assets/sector_4.png",          "assets/sector_5.png",    "assets/sector_6.png",   "assets/sector_7.png",
    "assets/sector_8.png",          "assets/sector_9.png",    "assets/sector_10.png",  "assets/gas_giant_1.png",
    "assets/gas_giant_2.png",       "assets/gas_giant_3.png", "assets/islands_1.png",  "assets/islands_2.png",
    "assets/islands_3.png",         "assets/no_atm_1.png",    "assets/no_atm_2.png",   "assets/no_atm_3.png",
    "assets/rings_1.png",           "assets/rings_2.png",     "assets/rings_3.png",    "assets/terran_d_1.png",
    "assets/terran_d_2.png",        "assets/terran_d_3.png",  "assets/terran_w_1.png", "assets/terran_w_2.png",
    "assets/terran_w_3.png",
};
}  // namespace planets_meta

namespace basic_textures
{
constexpr std::string_view background_texture_path = "assets/background_texture.png";
constexpr std::string_view menu_universe_button = "assets/menu_universe_button.png";
constexpr std::string_view sector_selection = "assets/sector_selection.png";
constexpr std::string_view sky_texture_path = "assets/sky.png";
constexpr std::string_view building_sprite_path = "assets/buildings.png";
}  // namespace basic_textures

namespace fonts
{
constexpr std::string_view main_font = "assets/fonts/arcade.ttf";
constexpr std::string_view secondary_font = "assets/fonts/secondary.ttf";
constexpr int main_font_size = 50;
constexpr int resource_font_size = 25;
}  // namespace fonts
namespace buttons_meta
{
constexpr int button_height = 75;
constexpr int button_width = 200;
constexpr int button_x_offset = 25;
constexpr int button_y_offset = 110;

}  // namespace buttons_meta

namespace resources_meta
{
constexpr std::string_view resources_path = "assets/resources.png";
constexpr int total_size = 256;
constexpr int single_size = 128;
}  // namespace resources_meta

#endif
