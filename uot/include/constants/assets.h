#ifndef ASSETS_H
#define ASSETS_H

// Planetoid spritesheets generated with
// https://deep-fold.itch.io/pixel-planet-generator

#include <array>
#include <string_view>
#include "size_settings.h"

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
    TERRAN_W_3,
    STAR_BLUE_GIANT_1,
    STAR_BLUE_GIANT_2,
    STAR_BLUE_GIANT_3,
    STAR_RED_GIANT_1,
    STAR_RED_GIANT_2,
    STAR_RED_GIANT_3,
    STAR_WHITE_DWARF_1,
    STAR_WHITE_DWARF_2,
    STAR_WHITE_DWARF_3,
    STAR_SUNNY_1,
    STAR_SUNNY_2,
    STAR_SUNNY_3,
    STAR_BLACK_HOLE_1,
    STAR_BLACK_HOLE_2,
    STAR_BLACK_HOLE_3,
    STAR_BROWN_DWARF_1,
    STAR_BROWN_DWARF_2,
    STAR_BROWN_DWARF_3,
    ASTEROID_1,
    ASTEROID_2,
    ASTEROID_3,
    PLANET_COLD_1,
    PLANET_COLD_2,
    PLANET_COLD_3,
    PLANET_NORMAL_1,
    PLANET_NORMAL_2,
    PLANET_NORMAL_3,
    PLANET_HOT_1,
    PLANET_HOT_2,
    PLANET_HOT_3,
    DARK_MATTER_1,
    DARK_MATTER_2,
    DARK_MATTER_3,
    UNDISCOVERED
};

namespace selection_meta
{
constexpr int num_selection_textures = 2;
constexpr int texture_width = 400;
constexpr int texture_height = 400;

}  // namespace selection_meta

namespace weapons_meta
{
constexpr std::string_view missile_path = "assets/missile.png";
constexpr int missile_w = 127;
constexpr int missile_h = 123;
constexpr std::string_view explosion1_path = "assets/explosion-1.png";
constexpr int explosion1_w = 256;
constexpr int explosion1_h = 32;
constexpr std::string_view explosion2_path = "assets/explosion-2.png";
constexpr int explosion2_w = 384;
constexpr int explosion2_h = 48;
}  // namespace weapons_meta

namespace planets_meta
{
constexpr float sector_multiplier = 0.3f;
constexpr int num_planets = 63;
constexpr int num_frames = 50;
constexpr int frame_width = 300;
constexpr int frame_height = 300;
constexpr int frame_duration = 5;

constexpr std::array<int, num_planets> texture_size = {
    300, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 100, 100, 100, 100, 100, 100, 100, 100, 100, 300,
    300, 300, 100, 100, 100, 100, 100, 100, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200,
    200, 200, 200, 200, 200, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 200};

constexpr std::array<std::string_view, num_planets> planet_texture_paths = {
    "assets/menu_planet_sheet.png",
    "assets/sector_1.png",
    "assets/sector_2.png",
    "assets/sector_3.png",
    "assets/sector_4.png",
    "assets/sector_5.png",
    "assets/sector_6.png",
    "assets/sector_7.png",
    "assets/sector_8.png",
    "assets/sector_9.png",
    "assets/sector_10.png",
    "assets/gas_giant_1.png",
    "assets/gas_giant_2.png",
    "assets/gas_giant_3.png",
    "assets/islands_1.png",
    "assets/islands_2.png",
    "assets/islands_3.png",
    "assets/no_atm_1.png",
    "assets/no_atm_2.png",
    "assets/no_atm_3.png",
    "assets/rings_1.png",
    "assets/rings_2.png",
    "assets/rings_3.png",
    "assets/terran_d_1.png",
    "assets/terran_d_2.png",
    "assets/terran_d_3.png",
    "assets/terran_w_1.png",
    "assets/terran_w_2.png",
    "assets/terran_w_3.png",
    "assets/star_blue_giant_1.png",
    "assets/star_blue_giant_2.png",
    "assets/star_blue_giant_3.png",
    "assets/star_red_giant_1.png",
    "assets/star_red_giant_2.png",
    "assets/star_red_giant_3.png",
    "assets/star_white_dwarf_1.png",
    "assets/star_white_dwarf_2.png",
    "assets/star_white_dwarf_3.png",
    "assets/star_sunny_1.png",
    "assets/star_sunny_2.png",
    "assets/star_sunny_3.png",
    "assets/star_black_hole_1.png",
    "assets/star_black_hole_2.png",
    "assets/star_black_hole_3.png",
    "assets/star_brown_dwaft_1.png",
    "assets/star_brown_dwaft_2.png",
    "assets/star_brown_dwaft_3.png",
    "assets/asteroid_1.png",
    "assets/asteroid_2.png",
    "assets/asteroid_3.png",
    "assets/planet_cold_1.png",
    "assets/planet_cold_2.png",
    "assets/planet_cold_3.png",
    "assets/planet_normal_1.png",
    "assets/planet_normal_2.png",
    "assets/planet_normal_3.png",
    "assets/planet_hot_1.png",
    "assets/planet_hot_2.png",
    "assets/planet_hot_3.png",
    "assets/dark_matter_1.png",
    "assets/dark_matter_2.png",
    "assets/dark_matter_3.png",
    "assets/undiscovered.png",
};
}  // namespace planets_meta

namespace basic_textures
{
constexpr std::string_view background_texture_path = "assets/background_texture.png";
constexpr std::string_view menu_universe_button = "assets/menu_universe_button.png";
constexpr std::string_view sector_selection = "assets/sector_selection.png";
constexpr std::string_view sky_texture_path = "assets/sky.png";
constexpr std::string_view sky_square_texture_path = "assets/sky_square.png";
constexpr std::string_view building_sprite_path = "assets/buildings.png";
constexpr std::string_view building_blur_sprite_path = "assets/buildings_blurred.png";
constexpr std::string_view fleet_sprite_path = "assets/fleet.png";
}  // namespace basic_textures

namespace fonts
{
constexpr std::string_view main_font = "assets/fonts/arcade.ttf";
constexpr std::string_view secondary_font = "assets/fonts/secondary.ttf";
constexpr int main_font_size = 50;
constexpr int resource_font_size = 25;
constexpr int action_button_font_size = 15;
constexpr int infobox_font_size = 15;
constexpr int secondary_font_size = 25;
}  // namespace fonts
namespace buttons_meta
{
constexpr int button_height = 75;
constexpr int button_width = 225;
constexpr int button_x_offset = 25;
constexpr int button_y_offset = 110;

}  // namespace buttons_meta

namespace resources_meta
{
constexpr std::string_view resources_path = "assets/resources.png";
constexpr int total_size = 256;
constexpr int single_size = 128 / 8;
}  // namespace resources_meta

namespace cabin_meta
{
constexpr std::string_view cabin_path = "assets/cabin.png";
constexpr int w = 384;
constexpr int h = 216;
}  // namespace cabin_meta

namespace buildings_meta
{
static constexpr int num_buildings = 5;
struct s_pos
{
    int x;
    int y;
    int w;
    int h;
};

// PlanetaryAdministration, ImprovedMetalsMine, MetalsMine, Farm,
// Greenhouses
static constexpr std::array<s_pos, num_buildings> sprite_positions{
    s_pos{247, 0, 89, 83}, s_pos{0, 100, 74, 78}, s_pos{73, 89, 74, 80}, s_pos{77, 170, 99, 80}, s_pos{344, 0, 96, 88}};
static constexpr int screen_unit = size_settings::planet_queue_area::width / 8;
static constexpr int w = 1.2 * screen_unit;
static constexpr int h = 1.2 * screen_unit;
static constexpr std::array<std::string_view, num_buildings> names{"Planetary Administration", "Improved Metals Mine",
                                                                   "Metals Mine", "Farm", "Greenhouses"};
static constexpr std::array<s_pos, num_buildings> render_positions{
    s_pos{1 * screen_unit, 2 * screen_unit, w, h}, s_pos{3 * screen_unit, 2 * screen_unit, w, h},
    s_pos{1 * screen_unit, 4 * screen_unit, w, h}, s_pos{3 * screen_unit, 4 * screen_unit, w, h},
    s_pos{5 * screen_unit, 4 * screen_unit, w, h},
};

constexpr int frame_width = 300;

constexpr int frame_height = 500;

}  // namespace buildings_meta

namespace audio_meta
{
constexpr std::string_view ambient_filename = "assets/audio/ambient.wav";

constexpr std::string_view click_filename = "assets/audio/click.wav";
constexpr std::string_view open_planet_filename = "assets/audio/open_planet.wav";
constexpr std::string_view open_ship_filename = "assets/audio/open_ship.wav";
constexpr std::string_view scanning_filename = "assets/audio/scanning.wav";

}  // namespace audio_meta

#endif
