#ifndef ASSETS_H
#define ASSETS_H

#include <string_view>

enum button_types {
    START_BUTTON = 0,
    EXIT_BUTTON = 1,
    GALAXY_BUTTON = 2
};

namespace basic_textures {
    constexpr std::string_view background_texture_path = "assets/background_texture.png";
    constexpr std::string_view menu_planet_texture_path = "assets/menu_planet_sheet.png";
    constexpr std::string_view menu_start_button_texture = "assets/menu_start_button.png";
    constexpr std::string_view menu_exit_button_texture = "assets/menu_exit_button.png";
}

namespace button_sets {
   constexpr int num_buttons = 3;

   static const int all_button[] = {
       START_BUTTON,
       EXIT_BUTTON,
       GALAXY_BUTTON
   };

   static const int menu_buttons[] = {
       START_BUTTON,
       EXIT_BUTTON
   };

   static const int universe_buttons[] = {

   };

   static const int galaxy_buttons[] = {

   };

   static const int planet_buttons[] = {

   };

   static const int ship_buttons[] = {

   };

}

#endif