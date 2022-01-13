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

struct resource_area : area_properties<1550, 40, 25, 800>
{
};

struct play_area : area_properties<1100, 700, 25, 50>
{
};

struct context_area : area_properties<350, 700, 1225, 50>
{
};

// PLANET VIEW
struct planet_queue_area : area_properties<400, 700, 100, 50>
{
};

struct planet_built_area : area_properties<400, 700, 600, 50>
{
};

struct planet_build_area : area_properties<400, 700, 1100, 50>
{
};

struct popup_menu_area : area_properties<400, 600, 600, 150>
{
};

struct tech_node_size : area_properties<250, 60, 8, 3>
{
};

struct tech_area : area_properties<1080, 680, 35, 60>
{
};

}  // namespace size_settings

#endif
