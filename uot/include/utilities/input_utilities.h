#ifndef INPUT_UTILITIES_H
#define INPUT_UTILITIES_H
#include "sdl_utilities.h"

namespace input_utilities
{
enum class uot_event_type
{
    motion_play,
    motion_resource,
    motion_context,
    motion_else,
    left_click_play,
    left_click_resource,
    left_click_context,
    left_click_else,
    right_click_play,
    right_click_resource,
    right_click_context,
    right_click_else,
    other
};

template <typename T, int X = T::x_offset, int Y = T::y_offset, int W = T::width, int H = T::height>
bool check_view_area_collision(const int x, const int y)
{
    // Mouse is left of the area
    if (x < X || x > X + W || y < Y || y > Y + H)
    {
        return false;
    }

    return true;
}

inline bool check_collision(const int x, const int y, const int box_x, const int box_y, const int w, const int h)
{
    // Mouse is left of the area
    if (x < box_x || x > box_x + w || y < box_y || y > box_y + h)
    {
        return false;
    }

    return true;
}

uot_event_type get_event_type(Uint32 event_type, SDL_MouseButtonEvent m, int x, int y);

}  // namespace input_utilities

#endif  // INPUT_UTILITIES_H
