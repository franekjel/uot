#include "rendering/rendering_planet.h"
#include <algorithm>
#include <iostream>
#include "ui_list.h"
#include "client_context.h"
#include "game_gui.h"
#include "game_resources.h"
#include "game_state.h"
#include "planet.h"
#include "rendering_common.h"
#include "utilities/input_utilities.h"


void rendering::render_planet_view::init() {
    static std::vector<std::string> test = {"one", "two", "three", "four", "five" };
    available = std::make_shared<ui_list_state>(test, generic_button{[&](client_context& context) {
        std::cout << "Action button clicked " << std::endl;
        if(available->selected_elem.has_value()) {
            std::cout << "Action button clicked on " << available->elems[available->selected_elem.value()] << std::endl;
        }
    }, "Action", {300, 100, 150, 50}});
}

void rendering::render_planet_view::_draw(client_context& context)
{
    render_background(context);
    // draw the above astronaut buttons
    auto state = context.getGameState();
    auto& gs = state.value;
    auto& gr = context.gr;
    auto& r = context.r;
    auto& gui = context.gui;

    // background
    sdl_utilities::set_render_viewport<size_settings::resource_area>(r.get());
    sdl_utilities::paint_background(r.get(), SDL_Color{0x00, 0x00, 0x00, 150});
    render_resource_bar(context);

    // draw the left, main game panel
    sdl_utilities::set_render_viewport<size_settings::planet_play_area>(r.get());
    sdl_utilities::paint_frame_textured(r.get(), SDL_Color{0xFF, 0xFF, 0xFF, 0xFF}, gr->sky_square_texture);
    sdl_utilities::set_custom_viewport<size_settings::planet_play_area, size_settings::frame_size>(r.get());
    rendering::render_planet_helper(context, 10.0f, size_settings::planet_play_area::width / 6,
                                    size_settings::planet_play_area::height / 1.6,
                                    gr->planetTextures[11 + context.gui->current_object.value()->id % 18]);
    sdl_utilities::set_custom_viewport<size_settings::planet_play_area, size_settings::frame_size>(r.get());
    sdl_utilities::paint_background(r.get(), SDL_Color{0x00, 0x00, 0x00, 150});

    // =======================================================
    // draw the right, information / GUI panel
    sdl_utilities::set_render_viewport<size_settings::planet_context_area>(r.get());
    sdl_utilities::paint_frame(r.get(), SDL_Color{0xFF, 0xFF, 0xFF, 0xFF}, SDL_Color{0x00, 0x00, 0x00, 0xFF});

    // render list with possible buildings
    // create info struct

    // render struct
    render_list(context, available);

}

rendering::view_t rendering::render_planet_view::_up() { return std::make_shared<render_sector_view>(); }

rendering::view_t rendering::render_planet_view::_down() { return std::make_shared<render_planet_view>(); }

void rendering::render_planet_view::_mouse_handler(client_context& context, Uint32 event_type, SDL_MouseButtonEvent m,
        
                                                   int x, int y)
{
    namespace iu = input_utilities;
    const auto et = input_utilities::get_planet_event_type(event_type, m, x, y);
    auto& curr_b = context.gui->current_building;
    if (et == iu::uot_event_type::planet_left_click_context)
    {
        using AreaType = size_settings::planet_context_area;
        x = x - AreaType::x_offset;
        y = y - AreaType::y_offset;

        if (iu::check_collision(
                x, y, available->action_button.pos.x, available->action_button.pos.y,
                available->action_button.pos.w, available->action_button.pos.h)) {

            available->action_button.clicked(context);
        }
        // handle list element clicks
        available->handle_click(x, y);

    }
}

void rendering::render_planet_view::_wheel_handler(client_context& context,
        int x, int y, int xmov, int ymov) { 
    namespace iu = input_utilities;
    const auto et = input_utilities::get_planet_scroll_type(x, y);
    if (et == iu::uot_event_type::planet_scroll_context)
    {
        using AreaType = size_settings::planet_context_area;
        x = x - AreaType::x_offset;
        y = y - AreaType::y_offset;

        available->offset = std::clamp(0, 
                static_cast<int>(available->offset + ymov), 
                static_cast<int>(available->elems.size() * 80));
    }
}

void rendering::render_planet_view::key_handler(client_context& context, Uint16 k)
{
    if (k == SDLK_ESCAPE)
    {
        context.gui->current_building.reset();
        context.view = _up();
    }
}
