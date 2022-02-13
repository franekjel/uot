#ifndef GAME_GUI_H
#define GAME_GUI_H

#include <chrono>
#include <iostream>
#include <optional>

#include <sector.h>
#include <sectorobject.h>
#include <technology.h>
#include "animation.h"
#include "assets.h"
#include "gui/buttons.h"
#include "planet.h"
#include "ship.h"

struct sector_client_info
{
    unsigned int planet_cnt;
    unsigned int deposit_cnt;
    unsigned int fleet_cnt;
};

struct game_gui
{
    // whenever we want to create a button, it should be done using this as id
    int user_button_id = 50;
    int planet_frame = 0;

    std::chrono::time_point<std::chrono::steady_clock> last_turn_time;

    std::optional<std::shared_ptr<Sector>> current_sector;
    std::optional<std::shared_ptr<SectorObject>> current_object;
    std::optional<std::shared_ptr<Fleet>> current_fleet;
    int current_fleet_info_offset = 0;
    std::optional<Building::BuildingType> current_building;
    std::optional<Building::BuildingType> focused_building;
    std::optional<int> focused_button;

    std::vector<popup_button> popup_buttons;
    std::vector<navigation_button> navigation_menu_buttons;
    std::vector<fleet_button> selected_fleet_buttons;

    std::optional<Technology::TechnologyType> hovered_tech;
    std::optional<Technology::TechnologyType> current_tech;
    SDL_Point tech_offset = {0, 0};
    SDL_Point tech_size = {0, 0};

    std::unordered_map<unsigned int, unsigned int> galaxy_stars_textures;

    std::map<unsigned int, std::unique_ptr<Animation>> fight_animations;
    unsigned int last_id = 1;

    int GetTextureIndex(std::shared_ptr<SectorObject> p);
    std::optional<std::shared_ptr<TTF_Font>> button_font;
    void set_button_font(std::shared_ptr<TTF_Font>& new_font) { button_font = new_font; }
    void reset_button_font() { button_font.reset(); }
    std::optional<int> button_border_width;
    std::optional<button_color> button_color_override;
    std::optional<sector_client_info> cur_sector_info;
    void do_gui_per_turn_update(client_context& context);

    unsigned int player_id_cache = 0;

    unsigned int current_design_id = 0;

    game_gui() {}

    int get_next_button_id() { return user_button_id++; }

    void reset_selection()
    {
        current_sector.reset();
        current_object.reset();
        current_fleet.reset();
        current_building.reset();
        focused_building.reset();
        focused_button.reset();
        fight_animations.clear();
    }
};

#endif
