#ifndef GAME_GUI_H
#define GAME_GUI_H

#include <sector.h>
#include <sectorobject.h>
#include <iostream>
#include <optional>
#include "assets.h"
#include "ui_button.h"

struct game_gui
{
    // whenever we want to create a button, it should be done using this as id
    int user_button_id = 50;
    int planet_frame = 0;

    std::optional<std::shared_ptr<Sector>> current_sector;
    std::optional<std::shared_ptr<SectorObject>> current_object;
    std::optional<int> focused_button;

    std::vector<ui_button> popup_buttons;

    game_gui() {}

    int get_next_button_id() { return user_button_id++; }
};

#endif
