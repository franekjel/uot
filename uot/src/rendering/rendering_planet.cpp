#include "rendering/rendering_planet.h"
#include <buildings.h>
#include <algorithm>
#include <iostream>
#include "client_context.h"
#include "game_gui.h"
#include "game_resources.h"
#include "game_state.h"
#include "msg_queue.h"
#include "planet.h"
#include "rendering_common.h"
#include "ui_list.h"
#include "uot_net_client.h"
#include "utilities/input_utilities.h"

template <typename E>
constexpr typename std::underlying_type<E>::type to_underlying(E e) noexcept
{
    return static_cast<typename std::underlying_type<E>::type>(e);
}

void rendering::render_planet_view::init(client_context& context)
{
    std::vector<std::string> v_build;
    std::vector<std::string> v_built;
    std::vector<std::string> v_queue;

    std::shared_ptr<Planet> pl = std::dynamic_pointer_cast<Planet>(context.gui->current_object.value());
    std::shared_ptr<InhabitableObject> io =
        std::dynamic_pointer_cast<InhabitableObject>(context.gui->current_object.value());
    std::shared_ptr<Star> st = std::dynamic_pointer_cast<Star>(context.gui->current_object.value());

    if (pl && pl->colony)
    {
        auto available_buildings = pl->colony->GetAvailableBuildings();
        for (const auto& [b, count] : available_buildings)
        {
            v_build.push_back(Buildings.at(b).name);
            _build.push_back(b);
        }

        for (const auto& b : pl->colony->buildings)
        {
            for (int i = 0; i < b.second; ++i)
            {
                v_built.push_back(Buildings.at(b.first).name);
                _built.push_back(b.first);
            }
        }

        for (const auto& b : pl->colony->building_queue)
        {
            v_queue.push_back(Buildings.at(b.type).name);
        }
    }

    build = std::make_shared<ui_list_state>(
        v_build,
        generic_button{[&, pl, io, st](client_context& context)
                       {
                           std::cout << "Action button clicked " << std::endl;
                           if (pl)
                           {
                               if (build->selected_elem.has_value())
                               {
                                   std::cout << "Action button clicked on "
                                             << build->elems[build->selected_elem.value()] << std::endl;
                                   auto mq = context.getActionQueue().value;
                                   auto t = _build[build->selected_elem.value()];
                                   mq->build_building(pl->colony->id, t);
                                   built->elems.push_back(build->elems[build->selected_elem.value()]);
                                   _built.push_back(_build[build->selected_elem.value()]);

                                   pl->colony->buildings[_build[build->selected_elem.value()]]++;
                               }
                           }
                           else if (io)
                           {
                           }
                           else if (st)
                           {
                           }
                       },
                       "BUILD",
                       {size_settings::planet_build_area::width / 2 - 140, 600, 280, 50}},
        300, 50, 50, 10);

    built = std::make_shared<ui_list_state>(
        v_built,
        generic_button{[&, pl, io, st](client_context& context)
                       {
                           std::cout << "Action button clicked " << std::endl;
                           if (built->selected_elem.has_value())
                           {
                               std::cout << "Action button clicked on " << built->elems[built->selected_elem.value()]
                                         << std::endl;
                           }
                       },
                       "UPGRADE",
                       {size_settings::planet_build_area::width / 2 - 140, 600, 280, 50}},
        300, 50, 50, 10);

    queue = std::make_shared<ui_list_state>(
        v_queue,
        generic_button{[&, pl, io, st](client_context& context)
                       {
                           std::cout << "Action button clicked " << std::endl;
                           if (queue->selected_elem.has_value())
                           {
                               std::cout << "Action button clicked on " << queue->elems[queue->selected_elem.value()]
                                         << std::endl;
                           }
                       },
                       "CANCEL",
                       {size_settings::planet_build_area::width / 2 - 140, 600, 280, 50}},
        300, 50, 50, 10);
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

    std::shared_ptr<Planet> pl = std::dynamic_pointer_cast<Planet>(context.gui->current_object.value());
    std::shared_ptr<InhabitableObject> io =
        std::dynamic_pointer_cast<InhabitableObject>(context.gui->current_object.value());
    std::shared_ptr<Star> st = std::dynamic_pointer_cast<Star>(context.gui->current_object.value());

    if (pl)
    {
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

        if (box.has_value())
        {
            rendering::render_building_info_box(context, box.value().type, box.value().x, box.value().y);
        }
    }
    else if (io)
    {
    }
    else if (st)
    {
    }
}

rendering::view_t rendering::render_planet_view::_up() { return std::make_shared<render_sector_view>(); }

rendering::view_t rendering::render_planet_view::_down(client_context& context)
{
    return std::make_shared<render_planet_view>();
}

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

        if (iu::check_collision(x, y, build->action_button.pos.x, build->action_button.pos.y,
                                build->action_button.pos.w, build->action_button.pos.h))
        {
            build->action_button.clicked(context);
            build->selected_elem.reset();
        }
        else
        {
            // handle list element clicks
            build->handle_click(x, y);
        }
    }

    if (et == iu::uot_event_type::planet_left_click_built)
    {
        using AreaType = size_settings::planet_built_area;
        x = x - AreaType::x_offset;
        y = y - AreaType::y_offset;

        if (iu::check_collision(x, y, built->action_button.pos.x, built->action_button.pos.y,
                                built->action_button.pos.w, built->action_button.pos.h))
        {
            built->action_button.clicked(context);
            built->selected_elem.reset();
        }
        else
        {
            // handle list element clicks
            built->handle_click(x, y);
        }
    }

    if (et == iu::uot_event_type::planet_left_click_queue)
    {
        using AreaType = size_settings::planet_queue_area;
        x = x - AreaType::x_offset;
        y = y - AreaType::y_offset;

        if (iu::check_collision(x, y, queue->action_button.pos.x, queue->action_button.pos.y,
                                queue->action_button.pos.w, queue->action_button.pos.h))
        {
            queue->action_button.clicked(context);
            queue->selected_elem.reset();
        }
        else
        {
            // handle list element clicks
            queue->handle_click(x, y);
        }
    }

    if (et == iu::uot_event_type::planet_motion_build)
    {
        using AreaType = size_settings::planet_build_area;
        int _x = x - AreaType::x_offset;
        int _y = y - AreaType::y_offset;

        auto res = build->handle_motion(_x, _y);
        if (res.has_value())
        {
            std::cout << "ustawiam box" << std::endl;
            box = {x, y, _build[res.value()]};
            return;
        }
        box.reset();
    }

    if (et == iu::uot_event_type::planet_motion_built)
    {
        using AreaType = size_settings::planet_built_area;
        auto _x = x - AreaType::x_offset;
        auto _y = y - AreaType::y_offset;

        auto res = built->handle_motion(_x, _y);
        if (res.has_value())
        {
            std::cout << "ustawiam box" << std::endl;
            box = {x, y, _built[res.value()]};
            return;
        }
        box.reset();
    }

    if (et == iu::uot_event_type::planet_motion_queue)
    {
        using AreaType = size_settings::planet_queue_area;
        x = x - AreaType::x_offset;
        y = y - AreaType::y_offset;

        auto res = queue->handle_motion(x, y);
        if (res.has_value())
        {
            std::cout << "ustawiam box" << std::endl;
            box = {x, y, _queue[res.value()]};
            return;
        }
        box.reset();
    }
}

void rendering::render_planet_view::_wheel_handler(client_context& context, int x, int y, int xmov, int ymov)
{
    namespace iu = input_utilities;
    const auto et = input_utilities::get_planet_scroll_type(x, y);
    if (et == iu::uot_event_type::planet_scroll_build)
    {
        build->offset =
            std::clamp(0, static_cast<int>(build->offset + 4 * ymov), static_cast<int>(build->elems.size() * 80));
    }

    if (et == iu::uot_event_type::planet_scroll_built)
    {
        built->offset =
            std::clamp(0, static_cast<int>(built->offset + 4 * ymov), static_cast<int>(built->elems.size() * 80));
    }

    if (et == iu::uot_event_type::planet_scroll_queue)
    {
        queue->offset =
            std::clamp(0, static_cast<int>(queue->offset + 4 * ymov), static_cast<int>(queue->elems.size() * 80));
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

void rendering::render_building_info_box(client_context& context, Building::BuildingType type, int x, int y)
{
    auto& r = context.r;
    auto& gr = context.gr;

    const auto& b = Buildings.at(type);
    sdl_utilities::set_viewport(r.get(), x, y, buildings_meta::frame_width, buildings_meta::frame_height);
    sdl_utilities::paint_background(r.get(), SDL_Color{0x00, 0x00, 0x00, 200});

    auto b_idx = type % buildings_meta::num_buildings;

    SDL_Rect s{buildings_meta::sprite_positions[b_idx].x, buildings_meta::sprite_positions[b_idx].y,
               buildings_meta::sprite_positions[b_idx].w, buildings_meta::sprite_positions[b_idx].h};

    SDL_Rect d{buildings_meta::frame_width / 2 - buildings_meta::sprite_positions[b_idx].w / 2, 0,
               buildings_meta::sprite_positions[b_idx].w, buildings_meta::sprite_positions[b_idx].h};

    SDL_RenderCopyEx(context.r.get(), context.gr->buildings_sprite.get(), &s, &d, 0, nullptr, SDL_FLIP_NONE);

    std::string cost_string = " COST: \n";
    cost_string += "  Work: " + std::to_string(int(Buildings.at(type).worker_weeks_cost)) + "\n";
    for (const auto& e : Buildings.at(type).cost)
    {
        cost_string +=
            "  " + std::string(resourceNames[to_underlying(e.first)]) + ": " + std::to_string(int(e.second)) + "\n";
    }

    cost_string += "\n";

    std::string production_string = " PRODUCTION: \n";
    for (const auto& e : Buildings.at(type).production)
    {
        production_string +=
            "  " + std::string(resourceNames[to_underlying(e.first)]) + ": " + std::to_string(int(e.second)) + "\n";
    }

    production_string += "\n";

    std::string upkeep_string = " UPKEEP: \n";
    for (const auto& e : Buildings.at(type).upkeep)
    {
        upkeep_string +=
            "  " + std::string(resourceNames[to_underlying(e.first)]) + ": " + std::to_string(int(e.second)) + "\n";
    }

    upkeep_string += "   Workers: " + std::to_string((int)Buildings.at(type).workers) + "\n\n";

    sdl_utilities::render_text(
        r.get(), gr->infobox_font,
        std::string(Buildings.at(type).description) + "\n\n" + cost_string + "\n" + production_string + upkeep_string,
        buildings_meta::frame_width / 2, buildings_meta::frame_height / 2, buildings_meta::frame_width,
        SDL_Color{0xFF, 0xFF, 0xFF, 0xFF});
}
