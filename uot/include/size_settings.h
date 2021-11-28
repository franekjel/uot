#ifndef SIZE_SETTINGS_H
#define SIZE_SETTINGS_H

namespace size_settings
{
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
struct button_area : area_properties<250, 300, 25, 50>
{
};
struct play_area : area_properties<800, 800, 350, 50>
{
};
struct context_area : area_properties<350, 800, 1225, 50>
{
};
}  // namespace size_settings

#endif