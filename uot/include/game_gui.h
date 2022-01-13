#ifndef GAME_GUI_H
#define GAME_GUI_H

#include <sector.h>
#include <technology.h>
#include <sectorobject.h>
#include <iostream>
#include <optional>
#include "assets.h"
#include "gui/buttons.h"
#include "planet.h"

struct game_gui
{
    // whenever we want to create a button, it should be done using this as id
    int user_button_id = 50;
    int planet_frame = 0;

    std::optional<std::shared_ptr<Sector>> current_sector;
    std::optional<std::shared_ptr<SectorObject>> current_object;
    std::optional<Building::BuildingType> current_building;
    std::optional<Building::BuildingType> focused_building;
    std::optional<int> focused_button;
    std::optional<Technology::TechnologyType> hovered_tech;
    std::optional<std::shared_ptr<Technology>> current_tech;

    std::vector<popup_button> popup_buttons;
    std::vector<navigation_button> navigation_menu_buttons;

    game_gui() {}

    int get_next_button_id() { return user_button_id++; }
};

#endif
