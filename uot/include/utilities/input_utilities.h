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
    // PLANET
    planet_motion_queue,
    planet_motion_built,
    planet_motion_build,
    planet_motion_resource,
    planet_motion_else,

    planet_left_click_queue,
    planet_left_click_built,
    planet_left_click_build,
    planet_left_click_info,
    planet_left_click_resource,
    planet_left_click_else,

    planet_right_click_queue,
    planet_right_click_built,
    planet_right_click_build,
    planet_right_click_info,
    planet_right_click_resource,
    planet_right_click_else,

    planet_scroll_queue,
    planet_scroll_built,
    planet_scroll_build,
    planet_scroll_info,
    planet_scroll_other,
    planet_scroll_resource,

    designer_scroll_available,
    designer_scroll_hull,
    designer_scroll_chosen,
    designer_scroll_info_text,
    designer_motion_available,
    designer_motion_chosen,
    designer_motion_hull,
    designer_left_click_available,
    designer_right_click_available,
    designer_left_click_chosen,
    designer_right_click_chosen,
    designer_right_click_hull,
    designer_left_click_hull,
    designer_left_click_info,
    designer_right_click_info,

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

inline bool check_collision_circle(const int x, const int y, const int circle_x, const int circle_y, const int r)
{
    const float dx = x - circle_x;
    const float dy = y - circle_y;
    return dx * dx + dy * dy <= r * r;
}

uot_event_type get_event_type(Uint32 event_type, SDL_MouseButtonEvent m, int x, int y);

uot_event_type get_planet_event_type(Uint32 event_type, SDL_MouseButtonEvent m, int x, int y);

uot_event_type get_designer_event_type(Uint32 event_type, SDL_MouseButtonEvent m, int x, int y);

uot_event_type get_planet_scroll_type(int x, int y);

uot_event_type get_designer_scroll_type(int x, int y);

uot_event_type get_planet_build_event_type(Uint32 event_type, SDL_MouseButtonEvent m, int x, int y);

uot_event_type get_planet_build_scroll_type(int x, int y);
}  // namespace input_utilities

#endif  // INPUT_UTILITIES_H
