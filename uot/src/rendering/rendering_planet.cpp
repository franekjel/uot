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
    static std::vector<std::string> v_build = {"one", "two", "three", "four", "five", "one", "two", "three", "four", "five", "one", "two", "three", "four", "five" };

    static std::vector<std::string> v_built = {"one", "two", "three", "four", "five", "one", "two"};

    static std::vector<std::string> v_queue = {"one", "two", "three"};

    build = std::make_shared<ui_list_state>(v_build, generic_button{[&](client_context& context) {
        std::cout << "Action button clicked " << std::endl;
        if(build->selected_elem.has_value()) {
            std::cout << "Action button clicked on " << build->elems[build->selected_elem.value()] << std::endl;
        }
    }, "BUILD", {size_settings::planet_build_area::width / 2 - 125, 600, 250, 50}}, 300, 50, 50, 10);

    built = std::make_shared<ui_list_state>(v_built, generic_button{[&](client_context& context) {
        std::cout << "Action button clicked " << std::endl;
        if(built->selected_elem.has_value()) {
            std::cout << "Action button clicked on " << built->elems[built->selected_elem.value()] << std::endl;
        }
    }, "UPGRADE", {size_settings::planet_build_area::width / 2 - 125, 600, 250, 50}}, 300, 50, 50, 10);

    queue = std::make_shared<ui_list_state>(v_queue, generic_button{[&](client_context& context) {
        std::cout << "Action button clicked " << std::endl;
        if(queue->selected_elem.has_value()) {
            std::cout << "Action button clicked on " << queue->elems[queue->selected_elem.value()] << std::endl;
        }
    }, "CANCEL", {size_settings::planet_build_area::width / 2 - 125, 600, 250, 50}}, 300, 50, 50, 10);
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

    // draw queue
    sdl_utilities::set_render_viewport<size_settings::planet_queue_area>(r.get());
    sdl_utilities::paint_frame_textured(r.get(), SDL_Color{0xFF, 0xFF, 0xFF, 0xFF}, gr->sky_square_texture);
    sdl_utilities::set_custom_viewport<size_settings::planet_queue_area, size_settings::frame_size>(r.get());
    rendering::render_planet_helper(context, 10.0f, size_settings::planet_queue_area::width / 6,
                                    size_settings::planet_queue_area::height / 1.6,
                                    gr->planetTextures[11 + context.gui->current_object.value()->id % 18]);
    sdl_utilities::set_custom_viewport<size_settings::planet_queue_area, size_settings::frame_size>(r.get());
    sdl_utilities::paint_background(r.get(), SDL_Color{0x00, 0x00, 0x00, 150});
    render_list(context, queue);

    // =======================================================
    // draw list with upgradeable (or not) buildings already present on the planet
    sdl_utilities::set_render_viewport<size_settings::planet_built_area>(r.get());
    sdl_utilities::paint_frame(r.get(), SDL_Color{0xFF, 0xFF, 0xFF, 0xFF}, SDL_Color{0x00, 0x00, 0x00, 0xFF});
    render_list(context, built);

    // draw list with upgradeable (or not) buildings already present on the planet
    sdl_utilities::set_render_viewport<size_settings::planet_build_area>(r.get());
    sdl_utilities::paint_frame(r.get(), SDL_Color{0xFF, 0xFF, 0xFF, 0xFF}, SDL_Color{0x00, 0x00, 0x00, 0xFF});
    render_list(context, build);

}

rendering::view_t rendering::render_planet_view::_up() { return std::make_shared<render_sector_view>(); }

rendering::view_t rendering::render_planet_view::_down() { return std::make_shared<render_planet_view>(); }

void rendering::render_planet_view::_mouse_handler(client_context& context, Uint32 event_type, SDL_MouseButtonEvent m,
        
                                                   int x, int y)
{
    namespace iu = input_utilities;
    const auto et = input_utilities::get_planet_event_type(event_type, m, x, y);
    auto& curr_b = context.gui->current_building;
    if (et == iu::uot_event_type::planet_left_click_build)
    {
        using AreaType = size_settings::planet_build_area;
        x = x - AreaType::x_offset;
        y = y - AreaType::y_offset;

        if (iu::check_collision(
                x, y, build->action_button.pos.x, build->action_button.pos.y,
                build->action_button.pos.w, build->action_button.pos.h)) {

            build->action_button.clicked(context);
            build->selected_elem.reset();
        } else {
        // handle list element clicks
            build->handle_click(x, y);
        }

    }

    if (et == iu::uot_event_type::planet_left_click_built)
    {
        using AreaType = size_settings::planet_built_area;
        x = x - AreaType::x_offset;
        y = y - AreaType::y_offset;

        if (iu::check_collision(
                x, y, built->action_button.pos.x, built->action_button.pos.y,
                built->action_button.pos.w, built->action_button.pos.h)) {

            built->action_button.clicked(context);
            built->selected_elem.reset();
        } else {
        // handle list element clicks
            built->handle_click(x, y);
        }

    }

    if (et == iu::uot_event_type::planet_left_click_queue)
    {
        using AreaType = size_settings::planet_queue_area;
        x = x - AreaType::x_offset;
        y = y - AreaType::y_offset;

        if (iu::check_collision(
                x, y, queue->action_button.pos.x, queue->action_button.pos.y,
                queue->action_button.pos.w, queue->action_button.pos.h)) {

            queue->action_button.clicked(context);
            queue->selected_elem.reset();
        } else {
        // handle list element clicks
            queue->handle_click(x, y);
        }

    }
}

void rendering::render_planet_view::_wheel_handler(client_context& context,
        int x, int y, int xmov, int ymov) { 
    namespace iu = input_utilities;
    const auto et = input_utilities::get_planet_scroll_type(x, y);
    if (et == iu::uot_event_type::planet_scroll_build)
    {
        build->offset = std::clamp(0,
                static_cast<int>(build->offset + 4 * ymov),
                static_cast<int>(build->elems.size() * 80));
    }

    if (et == iu::uot_event_type::planet_scroll_built) {
        built->offset = std::clamp(0,
                static_cast<int>(built->offset + 4 * ymov),
                static_cast<int>(built->elems.size() * 80));
    }

    if (et == iu::uot_event_type::planet_scroll_queue) {
        queue->offset = std::clamp(0,
                static_cast<int>(queue->offset + 4 * ymov),
                static_cast<int>(queue->elems.size() * 80));
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
