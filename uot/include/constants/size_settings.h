#ifndef SIZE_SETTINGS_H
#define SIZE_SETTINGS_H

namespace size_settings
{
constexpr int frame_size = 8;
template <int W, int H, int X, int Y>
struct area_properties
{
    static const int width = W;
    static const int height = H;
    static const int x_offset = X;
    static const int y_offset = Y;

   private:
    area_properties(){};
};

struct window_area : area_properties<1600, 900, 0, 0>
{
};

struct resource_area : area_properties<1560, 40, 20, 850>
{
};

struct play_area : area_properties<1200, 820, 20, 20>
{
};

struct context_area : area_properties<350, 820, 1230, 20>
{
};

// PLANET VIEW
struct planet_built_area : area_properties<400, 770, 1190, 10>
{
};

struct planet_queue_area : area_properties<400, 380, 10, 400>
{
};

struct planet_ships_queue_area : area_properties<400, 380, 10, 10>
{
};

struct planet_build_area : area_properties<400, 770, 1190, 10>
{
};

struct designer_available_area : area_properties<400, 770, 1190, 10>
{
};

struct designer_hull_area : area_properties<400, 240, 10, 10>
{
};

struct designer_chosen_area : area_properties<400, 460, 10, 300>
{
};

struct planet_ships_build_area : area_properties<400, 770, 10, 10>
{
};

struct planet_info_area : area_properties<700, 770, 450, 10>
{
};

struct designer_info_area : area_properties<700, 770, 450, 10>
{
};

struct designer_info_text_area : area_properties<700, 600, 450, 50>
{
};

struct planet_info_text_area : area_properties<700, 310, 450, 450>
{
};

struct popup_menu_area : area_properties<400, 600, 600, 150>
{
};

struct tech_node_size : area_properties<250, 60, 8, 3>
{
};

struct tech_area : area_properties<1180, 800, 30, 30>
{
};

struct fleet_info_area : area_properties<350, 300, 1225, 120>
{
};

}  // namespace size_settings

#endif
