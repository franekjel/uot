#include "input_utilities.h"

namespace input_utilities
{
uot_event_type get_event_type(Uint32 event_type, SDL_MouseButtonEvent m, int x, int y)
{
    switch (event_type)
    {
        case SDL_MOUSEMOTION:
        {
            if (check_view_area_collision<size_settings::play_area>(x, y))
            {
                return uot_event_type::motion_play;
            }

            if (check_view_area_collision<size_settings::resource_area>(x, y))
            {
                return uot_event_type::motion_resource;
            }

            if (check_view_area_collision<size_settings::context_area>(x, y))
            {
                return uot_event_type::motion_context;
            }
            return uot_event_type::other;
        }
        break;

        case SDL_MOUSEBUTTONDOWN:
        {
            if (check_view_area_collision<size_settings::play_area>(x, y))
            {
                return m.button == SDL_BUTTON_LEFT ? uot_event_type::left_click_play : uot_event_type::right_click_play;
            }

            if (check_view_area_collision<size_settings::resource_area>(x, y))
            {
                return m.button == SDL_BUTTON_LEFT ? uot_event_type::left_click_resource
                                                   : uot_event_type::right_click_resource;
            }

            if (check_view_area_collision<size_settings::context_area>(x, y))
            {
                return m.button == SDL_BUTTON_LEFT ? uot_event_type::left_click_context
                                                   : uot_event_type::right_click_context;
            }
            return m.button == SDL_BUTTON_LEFT ? uot_event_type::left_click_else : uot_event_type::right_click_else;
        }
        break;
        default:
            break;
    }
    return uot_event_type::other;
}

uot_event_type get_planet_build_event_type(Uint32 event_type, SDL_MouseButtonEvent m, int x, int y)
{
    switch (event_type)
    {
        case SDL_MOUSEMOTION:
        {
            if (check_view_area_collision<size_settings::planet_build_area>(x, y))
            {
                return uot_event_type::planet_motion_build;
            }
        }

        case SDL_MOUSEBUTTONDOWN:
        {
            if (check_view_area_collision<size_settings::planet_build_area>(x, y))
            {
                return m.button == SDL_BUTTON_LEFT ? uot_event_type::planet_left_click_build
                                                   : uot_event_type::planet_right_click_build;
            }

            if (check_view_area_collision<size_settings::planet_info_area>(x, y))
            {
                return m.button == SDL_BUTTON_LEFT ? uot_event_type::planet_left_click_info
                                                   : uot_event_type::planet_right_click_info;
            }
        }
    }

    return uot_event_type::other;
}

uot_event_type get_designer_event_type(Uint32 event_type, SDL_MouseButtonEvent m, int x, int y)
{
    switch (event_type)
    {
        case SDL_MOUSEMOTION:
        {
            if (check_view_area_collision<size_settings::designer_available_area>(x, y))
            {
                return uot_event_type::designer_motion_available;
            }

            if (check_view_area_collision<size_settings::designer_chosen_area>(x, y))
            {
                return uot_event_type::designer_motion_chosen;
            }

            if (check_view_area_collision<size_settings::designer_hull_area>(x, y))
            {
                return uot_event_type::designer_motion_hull;
            }
        }
        case SDL_MOUSEBUTTONDOWN:
        {
            if (check_view_area_collision<size_settings::designer_available_area>(x, y))
            {
                return m.button == SDL_BUTTON_LEFT ? uot_event_type::designer_left_click_available
                                                   : uot_event_type::designer_right_click_available;
            }

            if (check_view_area_collision<size_settings::designer_chosen_area>(x, y))
            {
                return m.button == SDL_BUTTON_LEFT ? uot_event_type::designer_left_click_chosen
                                                   : uot_event_type::designer_right_click_chosen;
            }

            if (check_view_area_collision<size_settings::designer_hull_area>(x, y))
            {
                return m.button == SDL_BUTTON_LEFT ? uot_event_type::designer_left_click_hull
                                                   : uot_event_type::designer_right_click_hull;
            }

            if (check_view_area_collision<size_settings::designer_info_area>(x, y))
            {
                return m.button == SDL_BUTTON_LEFT ? uot_event_type::designer_left_click_info
                                                   : uot_event_type::designer_right_click_info;
            }
        }
    }
    return uot_event_type::other;
}

uot_event_type get_planet_event_type(Uint32 event_type, SDL_MouseButtonEvent m, int x, int y)
{
    switch (event_type)
    {
        case SDL_MOUSEMOTION:
        {
            if (check_view_area_collision<size_settings::planet_queue_area>(x, y))
            {
                return uot_event_type::planet_motion_queue;
            }

            if (check_view_area_collision<size_settings::resource_area>(x, y))
            {
                return uot_event_type::planet_motion_resource;
            }

            if (check_view_area_collision<size_settings::planet_built_area>(x, y))
            {
                return uot_event_type::planet_motion_built;
            }

            return uot_event_type::planet_motion_else;
        }
        break;

        case SDL_MOUSEBUTTONDOWN:
        {
            if (check_view_area_collision<size_settings::planet_queue_area>(x, y))
            {
                return m.button == SDL_BUTTON_LEFT ? uot_event_type::planet_left_click_queue
                                                   : uot_event_type::planet_right_click_queue;
            }

            if (check_view_area_collision<size_settings::resource_area>(x, y))
            {
                return m.button == SDL_BUTTON_LEFT ? uot_event_type::planet_left_click_resource
                                                   : uot_event_type::planet_right_click_resource;
            }

            if (check_view_area_collision<size_settings::planet_built_area>(x, y))
            {
                return m.button == SDL_BUTTON_LEFT ? uot_event_type::planet_left_click_built
                                                   : uot_event_type::planet_right_click_built;
            }
            if (check_view_area_collision<size_settings::planet_info_area>(x, y))
            {
                return m.button == SDL_BUTTON_LEFT ? uot_event_type::planet_left_click_info
                                                   : uot_event_type::planet_right_click_info;
            }
            return m.button == SDL_BUTTON_LEFT ? uot_event_type::planet_left_click_else
                                               : uot_event_type::planet_right_click_else;
        }
        break;
        default:
            break;
    }
    return uot_event_type::other;
}

uot_event_type get_planet_build_scroll_type(int x, int y)
{
    if (check_view_area_collision<size_settings::planet_build_area>(x, y))
    {
        return uot_event_type::planet_scroll_build;
    }

    return uot_event_type::planet_scroll_other;
}

uot_event_type get_designer_scroll_type(int x, int y)
{
    if (check_view_area_collision<size_settings::designer_available_area>(x, y))
    {
        return uot_event_type::designer_scroll_available;
    }

    if (check_view_area_collision<size_settings::designer_chosen_area>(x, y))
    {
        return uot_event_type::designer_scroll_chosen;
    }

    if (check_view_area_collision<size_settings::designer_hull_area>(x, y))
    {
        return uot_event_type::designer_scroll_hull;
    }

    if (check_view_area_collision<size_settings::designer_info_text_area>(x, y))
    {
        return uot_event_type::designer_scroll_info_text;
    }

    return uot_event_type::other;
}

uot_event_type get_planet_scroll_type(int x, int y)
{
    if (check_view_area_collision<size_settings::planet_queue_area>(x, y))
    {
        return uot_event_type::planet_scroll_queue;
    }

    if (check_view_area_collision<size_settings::resource_area>(x, y))
    {
        return uot_event_type::planet_scroll_resource;
    }

    if (check_view_area_collision<size_settings::planet_built_area>(x, y))
    {
        return uot_event_type::planet_scroll_built;
    }
    if (check_view_area_collision<size_settings::planet_build_area>(x, y))
    {
        return uot_event_type::planet_scroll_build;
    }

    if (check_view_area_collision<size_settings::planet_info_text_area>(x, y))
    {
        return uot_event_type::planet_scroll_info;
    }
    return uot_event_type::planet_scroll_other;
}
}  // namespace input_utilities
