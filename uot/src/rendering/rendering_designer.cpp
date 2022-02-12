#include "rendering/rendering_designer.h"
#include "rendering/rendering_universe.h"
#include "client_context.h"
#include "player.h"
#include "msg_queue.h"
#include "input_utilities.h"

using namespace rendering;

view_t render_designer_view::_up() {
    return std::make_shared<render_universe_view>();
}

view_t render_designer_view::_down(client_context& context) {
    return std::make_shared<render_designer_view>();
}

void render_designer_view::_mouse_handler(client_context& context, Uint32 event_type, SDL_MouseButtonEvent m, int x, int y) {
        namespace iu = input_utilities;
        const auto et = input_utilities::get_designer_event_type(event_type, m, x, y);

        if (et == iu::uot_event_type::designer_left_click_available)
        {
            using AreaType = size_settings::planet_build_area;
            x = x - AreaType::x_offset;
            y = y - AreaType::y_offset;

            if (iu::check_collision(x, y, modules_available->action_button.pos.x, modules_available->action_button.pos.y,
                                    modules_available->action_button.pos.w, modules_available->action_button.pos.h))
            {
                modules_available->action_button.clicked(context);
                modules_available->selected_elem.reset();
            }
            else
            {
                // handle list element clicks
                modules_available->handle_timed_click(context, x, y);
            }
        }

}

void render_designer_view::_wheel_handler(client_context& context, int x, int y, int xmov, int ymov) {
    namespace iu = input_utilities;
    const auto et = input_utilities::get_designer_scroll_type(x, y);
    if (et == iu::uot_event_type::designer_scroll_available)
    {
        modules_available->offset =
            std::clamp(0, static_cast<int>(modules_available->offset + 4 * ymov), static_cast<int>(modules_available->elems.size() * 80));
    }

    if (et == iu::uot_event_type::designer_scroll_chosen)
    {
        modules_chosen->offset =
            std::clamp(0, static_cast<int>(modules_chosen->offset + 4 * ymov), static_cast<int>(modules_chosen->elems.size() * 80));
    }

}

void render_designer_view::key_handler(client_context& context, Uint16 k) {
    if (k == SDLK_ESCAPE)
    {
        context.gui->current_building.reset();
        context.view = _up();
    }
}

void render_designer_view::_draw(client_context& context) {
    refresh_lists(context);

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

    sdl_utilities::set_render_viewport<size_settings::designer_available_area>(r.get());
    sdl_utilities::paint_frame(r.get(), SDL_Color{0xFF, 0xFF, 0xFF, 0xFF}, SDL_Color{0x00, 0x00, 0x00, 0xFF});
    render_list(context, modules_available);

    sdl_utilities::set_render_viewport<size_settings::designer_chosen_area>(r.get());
    sdl_utilities::paint_frame(r.get(), SDL_Color{0xFF, 0xFF, 0xFF, 0xFF}, SDL_Color{0x00, 0x00, 0x00, 0xFF});
    render_list(context, modules_chosen);
}

void render_designer_view::init(client_context& context) {
    std::vector<std::string> v_available;
    std::vector<std::string> v_chosen;

    auto gs = context.getGameState();
    for(auto& m : Modules) {
        v_available.push_back(m.second.name);
        _available.push_back(m.first);
    }

    modules_available = std::make_shared<ui_list_state>(
        v_available,
        generic_button{[&](client_context& context)
                       {
                       if (modules_available->selected_elem.has_value())
                       {
                       auto v = modules_available->selected_elem.value();
                       _chosen.push_back(_available[v]);
                       modules_chosen->elems.push_back(
                           modules_available->elems[v]);
                       }
                       },
                       "ADD",
                       {size_settings::planet_build_area::width / 2 - 140, 670, 280, 50}},
        300, 50, 50, 10);

    modules_chosen = std::make_shared<ui_list_state>(
        v_chosen,
        generic_button{[&](client_context& context)
                       {
                           auto gs = context.getGameState();
                           if (modules_chosen->selected_elem.has_value())
                           {
                               auto v = modules_chosen->selected_elem.value();
                               auto mq = context.getActionQueue().value;

                           }
                       },
                       "BUILD",
                       {size_settings::planet_build_area::width / 2 - 180, 670, 360, 50}},
        300, 50, 50, 10);

}

inline void render_designer_view::refresh_lists(client_context &context) {

}
