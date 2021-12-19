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

uot_event_type get_planet_event_type(Uint32 event_type, SDL_MouseButtonEvent m, int x, int y)
{
    switch (event_type)
    {
        case SDL_MOUSEMOTION:
        {
            if (check_view_area_collision<size_settings::planet_play_area>(x, y))
            {
                return uot_event_type::planet_motion_play;
            }

            if (check_view_area_collision<size_settings::resource_area>(x, y))
            {
                return uot_event_type::planet_motion_resource;
            }

            if (check_view_area_collision<size_settings::planet_context_area>(x, y))
            {
                return uot_event_type::planet_motion_context;
            }
            return uot_event_type::planet_motion_else;
        }
        break;

        case SDL_MOUSEBUTTONDOWN:
        {
            if (check_view_area_collision<size_settings::planet_play_area>(x, y))
            {
                return m.button == SDL_BUTTON_LEFT ? uot_event_type::planet_left_click_play
                                                   : uot_event_type::planet_right_click_play;
            }

            if (check_view_area_collision<size_settings::resource_area>(x, y))
            {
                return m.button == SDL_BUTTON_LEFT ? uot_event_type::planet_left_click_resource
                                                   : uot_event_type::planet_right_click_resource;
            }

            if (check_view_area_collision<size_settings::planet_context_area>(x, y))
            {
                return m.button == SDL_BUTTON_LEFT ? uot_event_type::planet_left_click_context
                                                   : uot_event_type::planet_right_click_context;
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
}  // namespace input_utilities
