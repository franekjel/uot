#ifndef GAME_GUI_H
#define GAME_GUI_H

#include <chrono>
#include <iostream>
#include <optional>

#include <sector.h>
#include <sectorobject.h>
#include <technology.h>
#include "assets.h"
#include "gui/buttons.h"
#include "planet.h"

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

    std::optional<Technology::TechnologyType> hovered_tech;
    std::optional<Technology::TechnologyType> current_tech;
    SDL_Point tech_offset = {0, 0};
    SDL_Point tech_size = {0, 0};

    game_gui() {}

    int get_next_button_id() { return user_button_id++; }
};

#endif
