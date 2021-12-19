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

struct planet_play_area : area_properties<750, 700, 25, 50>
{
};

struct planet_context_area : area_properties<700, 700, 850, 50>
{
};

struct popup_menu_area : area_properties<400, 600, 600, 150>
{
};

}  // namespace size_settings

#endif
