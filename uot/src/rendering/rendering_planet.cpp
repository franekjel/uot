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

std::array<std::string, 3> planet_names{
    "Temperate Planet",  // Earth-like
    "Cold Planet",       // a bit colder
    "Hot Planet"         // a bit hotter
};

inline std::string get_planet_info(std::shared_ptr<Planet>& pl)
{
    std::string info;
    info += "Civilians: " + std::to_string(static_cast<int>(pl->colony->population)) + "\n";
    info += "Soldiers: " + std::to_string(static_cast<int>(pl->colony->soldiers)) + "\n\n";

    info += "Gains:\n";
    info += rendering::resource_to_text(pl->colony->GetColonyGains()) + "\n";

    info += "Expenses:\n";
    info += rendering::resource_to_text(pl->colony->GetColonyExpenses()) + "\n";

    info += "Features:\n";
    for (auto& f : pl->planetary_features)
    {
        const auto& feat = PlanetaryFeaturesTypes.at(f.first);
        info += feat.name + "(" + std::to_string(f.second) + ")\n";
    }

    return info;
}

void rendering::render_planet_view::render_planet_info(const client_context& context)
{
    auto& r = context.r;
    auto& gr = context.gr;
    auto& gui = context.gui;

    auto pl = std::dynamic_pointer_cast<Planet>(gui->current_object.value());
    auto io = std::dynamic_pointer_cast<InhabitableObject>(gui->current_object.value());
    auto st = std::dynamic_pointer_cast<Star>(gui->current_object.value());

    std::string name =
        pl   ? planet_names[static_cast<int>(pl->climate)]
        : io ? (std::string(resourceNames[static_cast<int>(io->resource_deposit.begin()->first)]) + " Object")
             : "Star";

    sdl_utilities::render_text_center(r.get(), gr->main_font, name.c_str(), size_settings::planet_info_area::width / 2,
                                      fonts::main_font_size / 2 + 30, size_settings::planet_info_area::width - 50,
                                      {0xFF, 0xFF, 0xFF, 0xFF});

    // render planet here again
    int textureIdx = gui->GetTextureIndex(gui->current_object.value());
    const bool biggie = planets_meta::texture_size[textureIdx] == 300;
    rendering::render_planet_helper(context, biggie ? 1.0 : 3.0, size_settings::planet_info_area::width / 2, 250,
                                    gr->planetTextures[textureIdx]);

    if (pl)
    {
        sdl_utilities::set_render_viewport<size_settings::planet_info_text_area>(r.get());

        const auto info = get_planet_info(pl);
        if (info.length() > 0)
        {
            sdl_utilities::render_text_center(r.get(), gr->secondary_font, info,
                                              size_settings::planet_info_area::width / 2,
                                              size_settings::planet_info_text_area::height / 2 + info_offset,
                                              size_settings::planet_info_area::width - 50, {0xFF, 0xFF, 0xFF, 0xFF});
        }

        int x, y;
        SDL_GetMouseState(&x, &y);

        x = x - size_settings::planet_info_area::x_offset;
        y = y - size_settings::planet_info_area::y_offset;

        if (x >= 0 && y >= 0 &&
            input_utilities::check_collision_circle(x, y, size_settings::planet_info_area::width / 2, 250,
                                                    planets_meta::frame_height / 2))
        {
            // horizontal
            const auto longer = planets_meta::frame_height / 2;
            const auto shorter = planets_meta::frame_height / 10;
            SDL_Rect hor{size_settings::planet_info_area::width / 2 - longer / 2, 250 - shorter / 2, longer, shorter};

            SDL_Rect ver{size_settings::planet_info_area::width / 2 +
                             (current_sub == planet_sub::queue ? longer / 2 : -longer / 2) - shorter / 2,
                         250 - longer / 4, shorter, longer / 2};

            SDL_SetRenderDrawColor(context.r.get(), 0xFF, 0xFF, 0xFF, 0xFF);
            sdl_utilities::set_render_viewport<size_settings::planet_info_area>(r.get());
            SDL_RenderFillRect(context.r.get(), &hor);
            SDL_RenderFillRect(context.r.get(), &ver);
        }
    }
}

void rendering::render_planet_view::init(client_context& context)
{
    std::vector<std::string> v_build;
    std::vector<std::string> v_built;
    std::vector<std::string> v_queue;

    std::vector<std::string> v_ship_build;
    std::vector<std::string> v_ship_built;
    std::vector<std::string> v_ship_queue;

    std::shared_ptr<Planet> pl = std::dynamic_pointer_cast<Planet>(context.gui->current_object.value());
    std::shared_ptr<InhabitableObject> io =
        std::dynamic_pointer_cast<InhabitableObject>(context.gui->current_object.value());
    std::shared_ptr<Star> st = std::dynamic_pointer_cast<Star>(context.gui->current_object.value());

    if (pl && pl->colony)
    {
        auto available_buildings = pl->colony->GetAvailableBuildings();
        for (const auto& [b, count] : available_buildings)
        {
            if (count > 0)
            {
                v_build.push_back(Buildings.at(b).name);
                _build.push_back(b);
            }
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
            v_queue.push_back(Buildings.at(b.type).name + "  " + std::to_string(b.worker_week_units_left));
            _queue.push_back(b.type);
        }

        auto gs = context.getGameState();
        for (auto& sd : gs.value->player->ship_designs)
        {
            v_ship_build.push_back(sd.second->name);
            _ships_build.push_back(sd.second->id);
        }

        for (auto& sd : pl->colony->ship_building_queue)
        {
            v_ship_queue.push_back(sd.design->name + " " + std::to_string(sd.worker_week_units_left));
            _ships_queue.push_back(sd.design->id);
        }
    }

    build = std::make_shared<ui_list_state>(
        v_build,
        generic_button{[&, pl, io, st](client_context& context)
                       {
                           if (pl)
                           {
                               if (build->selected_elem.has_value())
                               {
                                   auto mq = context.getActionQueue().value;
                                   auto t = _build[build->selected_elem.value()];
                                   mq->build_building(pl->colony->id, t);

                                   // dodać sprawdzanie czy zasoby pozwalają na
                                   // dodanie tego budynku
                                   _queue.push_back(t);

                                   queue->elems.push_back(
                                       Buildings.at(t).name + " " +
                                       std::to_string(static_cast<int>(Buildings.at(t).worker_weeks_cost)));
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
                       {size_settings::planet_build_area::width / 2 - 140, 670, 280, 50}},
        300, 50, 50, 10);

    ships_build = std::make_shared<ui_list_state>(
        v_ship_build,
        generic_button{[&, pl, io, st](client_context& context)
                       {
                           auto gs = context.getGameState();

                           if (pl)
                           {
                               if (ships_build->selected_elem.has_value())
                               {
                                   auto mq = context.getActionQueue().value;
                                   auto d_id = _ships_build[ships_build->selected_elem.value()];
                                   auto pl_id = pl->id;
                                   mq->build_ship(d_id, pl_id);

                                   _ships_queue.push_back(d_id);
                                   ships_queue->elems.push_back(
                                       gs.value->player->ship_designs.at(d_id)->name + " " +
                                       std::to_string(static_cast<int>(
                                           gs.value->player->ship_designs.at(d_id)->worker_weeks_cost)));
                               }
                           }
                           else if (io)
                           {
                           }
                           else if (st)
                           {
                           }
                       },
                       "BUILD SHIP",
                       {size_settings::planet_build_area::width / 2 - 180, 670, 360, 50}},
        300, 50, 50, 10);

    built = std::make_shared<ui_list_state>(
        v_built,
        generic_button{[&, pl, io, st](client_context& context)
                       {
                           if (built->selected_elem.has_value())
                           {
                               auto mq = context.getActionQueue().value;
                               auto t = _built[built->selected_elem.value()];
                               auto up = Buildings.at(t).upgrade;
                               mq->upgrade_building(pl->colony->id, t, up);
                               pl->colony->buildings[t]--;

                               _queue.push_back(Buildings.at(t).upgrade);
                               queue->elems.push_back(
                                   Buildings.at(up).name + " " +
                                   std::to_string(static_cast<int>(Buildings.at(up).worker_weeks_cost)));
                           }
                       },
                       "UPGRADE",
                       {size_settings::planet_built_area::width / 2 - 140, 670, 280, 50}},
        300, 50, 50, 10);

    queue = std::make_shared<ui_list_state>(
        v_queue,
        generic_button{[&, pl, io, st](client_context& context)
                       {
                           if (queue->selected_elem.has_value())
                           {
                           }
                       },
                       "CANCEL",
                       {size_settings::planet_queue_area::width / 2 - 140, 310, 280, 50}},
        300, 50, 50, 10);

    ships_queue = std::make_shared<ui_list_state>(
        v_ship_queue,
        generic_button{[&, pl, io, st](client_context& context)
                       {
                           if (queue->selected_elem.has_value())
                           {
                           }
                       },
                       "CANCEL",
                       {size_settings::planet_queue_area::width / 2 - 140, 310, 280, 50}},
        300, 50, 50, 10);
}

inline void rendering::render_planet_view::refresh_lists(client_context& context, std::shared_ptr<Planet>& pl)
{
    build->elems.clear();
    built->elems.clear();
    queue->elems.clear();
    ships_build->elems.clear();
    ships_queue->elems.clear();
    _build.clear();
    _built.clear();
    _queue.clear();
    _ships_build.clear();
    _ships_queue.clear();

    auto available_buildings = pl->colony->GetAvailableBuildings();
    for (const auto& [b, count] : available_buildings)
    {
        if (count > 0)
        {
            if (b == Building::Soldier)
                if (pl->colony->buildings.count(Building::MilitaryTrainingCentre) == 0 ||
                    pl->colony->buildings.at(Building::MilitaryTrainingCentre) == 0)
                    continue;
            build->elems.push_back(Buildings.at(b).name);
            _build.push_back(b);
        }
    }

    for (const auto& b : pl->colony->buildings)
    {
        for (int i = 0; i < b.second; ++i)
        {
            built->elems.push_back(Buildings.at(b.first).name);
            _built.push_back(b.first);
        }
    }

    for (const auto& b : pl->colony->building_queue)
    {
        queue->elems.push_back(Buildings.at(b.type).name + "  " +
                               std::to_string(static_cast<int>(b.worker_week_units_left)));
        _queue.push_back(b.type);
    }

    auto gs = context.getGameState();
    for (auto& sd : gs.value->player->ship_designs)
    {
        ships_build->elems.push_back(sd.second->name);
        _ships_build.push_back(sd.second->id);
    }

    for (auto& sd : pl->colony->ship_building_queue)
    {
        ships_queue->elems.push_back(sd.design->name);
        _ships_queue.push_back(sd.design->id);
    }
}

inline void rendering::render_planet_view::render_queue_lists(client_context& context)
{
    auto& r = context.r;

    sdl_utilities::set_render_viewport<size_settings::planet_queue_area>(r.get());
    sdl_utilities::paint_frame(r.get(), SDL_Color{0xFF, 0xFF, 0xFF, 0xFF}, SDL_Color{0x00, 0x00, 0x00, 0xFF});
    render_list(context, queue, false);

    sdl_utilities::set_render_viewport<size_settings::planet_built_area>(r.get());
    sdl_utilities::paint_frame(r.get(), SDL_Color{0xFF, 0xFF, 0xFF, 0xFF}, SDL_Color{0x00, 0x00, 0x00, 0xFF});
    render_list(context, built);

    sdl_utilities::set_render_viewport<size_settings::planet_ships_queue_area>(r.get());
    sdl_utilities::paint_frame(r.get(), SDL_Color{0xFF, 0xFF, 0xFF, 0xFF}, SDL_Color{0x00, 0x00, 0x00, 0xFF});
    render_list(context, ships_queue);
}

inline void rendering::render_planet_view::render_build_lists(client_context& context)
{
    auto& r = context.r;

    sdl_utilities::set_render_viewport<size_settings::planet_build_area>(r.get());
    sdl_utilities::paint_frame(r.get(), SDL_Color{0xFF, 0xFF, 0xFF, 0xFF}, SDL_Color{0x00, 0x00, 0x00, 0xFF});
    render_list(context, build);

    sdl_utilities::set_render_viewport<size_settings::planet_ships_build_area>(r.get());
    sdl_utilities::paint_frame(r.get(), SDL_Color{0xFF, 0xFF, 0xFF, 0xFF}, SDL_Color{0x00, 0x00, 0x00, 0xFF});
    render_list(context, ships_build);
}

void rendering::render_planet_view::_draw(client_context& context)
{
    std::shared_ptr<Planet> pl = std::dynamic_pointer_cast<Planet>(context.gui->current_object.value());
    std::shared_ptr<InhabitableObject> io =
        std::dynamic_pointer_cast<InhabitableObject>(context.gui->current_object.value());
    std::shared_ptr<Star> st = std::dynamic_pointer_cast<Star>(context.gui->current_object.value());

    if (pl && pl->colony)
    {
        refresh_lists(context, pl);
    }

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

    if (pl)
    {
        current_sub == planet_sub::queue ? render_queue_lists(context) : render_build_lists(context);

        sdl_utilities::set_render_viewport<size_settings::planet_info_area>(r.get());
        sdl_utilities::paint_background(r.get(), SDL_Color{0x00, 0x00, 0x00, 200});

        render_planet_info(context);

        if (box.has_value())
        {
            if (box->building_type.has_value())
            {
                rendering::render_building_info_box(context, box.value().building_type.value(), box.value().x,
                                                    box.value().y);
            }

            if (box->design_id.has_value())
            {
                rendering::render_ship_info_box(context, box.value().design_id.value(), box.value().x, box.value().y);
            }
        }
    }
    else if (io)
    {
        sdl_utilities::set_render_viewport<size_settings::planet_info_area>(r.get());
        sdl_utilities::paint_background(r.get(), SDL_Color{0x00, 0x00, 0x00, 200});

        render_planet_info(context);
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
    const auto et = current_sub == planet_sub::queue
                        ? input_utilities::get_planet_event_type(event_type, m, x, y)
                        : input_utilities::get_planet_build_event_type(event_type, m, x, y);

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
            build->handle_timed_click(context, x, y);
        }
    }

    if (et == iu::uot_event_type::planet_left_click_ship_build)
    {
        using AreaType = size_settings::planet_ships_build_area;
        x = x - AreaType::x_offset;
        y = y - AreaType::y_offset;

        if (iu::check_collision(x, y, ships_build->action_button.pos.x, ships_build->action_button.pos.y,
                                ships_build->action_button.pos.w, ships_build->action_button.pos.h))
        {
            ships_build->action_button.clicked(context);
            ships_build->selected_elem.reset();
        }
        else
        {
            // handle list element clicks
            ships_build->handle_timed_click(context, x, y);
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
            built->handle_timed_click(context, x, y);
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
            queue->handle_timed_click(context, x, y);
        }
    }

    if (et == iu::uot_event_type::planet_left_click_ship_queue)
    {
        using AreaType = size_settings::planet_ships_queue_area;
        x = x - AreaType::x_offset;
        y = y - AreaType::y_offset;

        if (iu::check_collision(x, y, ships_queue->action_button.pos.x, ships_queue->action_button.pos.y,
                                ships_queue->action_button.pos.w, ships_queue->action_button.pos.h))
        {
            ships_queue->action_button.clicked(context);
            ships_queue->selected_elem.reset();
        }
        else
        {
            // handle list element clicks
            ships_queue->handle_timed_click(context, x, y);
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
            box = {x, y, _build[res.value()]};
            return;
        }
        box.reset();
    }

    if (et == iu::uot_event_type::planet_motion_ship_build)
    {
        using AreaType = size_settings::planet_ships_build_area;
        int _x = x - AreaType::x_offset;
        int _y = y - AreaType::y_offset;

        auto res = ships_build->handle_motion(_x, _y);
        if (res.has_value())
        {
            box = {x, y, {}, _ships_build[res.value()]};
            return;
        }
        box.reset();
    }

    if (et == iu::uot_event_type::planet_motion_ship_queue)
    {
        using AreaType = size_settings::planet_ships_queue_area;
        int _x = x - AreaType::x_offset;
        int _y = y - AreaType::y_offset;

        auto res = ships_queue->handle_motion(_x, _y);
        if (res.has_value())
        {
            box = {x, y, {}, _ships_build[res.value()]};
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
            box = {x, y, _queue[res.value()]};
            return;
        }
        box.reset();
    }

    if (et == iu::uot_event_type::planet_left_click_info)
    {
        using AreaType = size_settings::planet_info_area;
        x = x - AreaType::x_offset;
        y = y - AreaType::y_offset;

        if (input_utilities::check_collision_circle(x, y, size_settings::planet_info_area::width / 2, 250,
                                                    planets_meta::frame_height / 2))
        {
            current_sub = current_sub == planet_sub::build ? planet_sub::queue : planet_sub::build;
        }
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

    if (et == iu::uot_event_type::planet_scroll_ship_queue)
    {
        ships_queue->offset = std::clamp(0, static_cast<int>(ships_queue->offset + 4 * ymov),
                                         static_cast<int>(ships_queue->elems.size() * 80));
    }

    if (et == iu::uot_event_type::planet_scroll_ship_build)
    {
        ships_build->offset = std::clamp(0, static_cast<int>(ships_build->offset + 4 * ymov),
                                         static_cast<int>(ships_build->elems.size() * 80));
    }

    if (et == iu::uot_event_type::planet_scroll_info)
    {
        info_offset += 4 * ymov;
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

inline std::string get_ship_info(const std::shared_ptr<ShipDesign>& design)
{
    std::string i = design->name + "\n\n";
    i += hull_type_to_design_name.at(design->hull_type) + " hull\n\n";

    for (const auto& [m, count] : design->inside)
        i += Modules.at(m).name + "(" + std::to_string(count) + ")\n";
    for (const auto& [m, count] : design->sides)
        i += Modules.at(m).name + "(" + std::to_string(count) + ")\n";

    i += "\nCost:\n";
    i += rendering::resource_to_text(design->cost);
    i += "  work:" + rendering::f2s(design->worker_weeks_cost);
    i += "\nUpkeep:\n";
    i += rendering::resource_to_text(design->upkeep);
    return i;
}

inline std::string get_building_info(Building::BuildingType type)
{
    std::string cost_string = " Cost: \n";
    cost_string += "   Work: " + std::to_string(int(Buildings.at(type).worker_weeks_cost)) + "\n";
    cost_string += rendering::resource_to_text(Buildings.at(type).cost, "   ");

    std::string production_string =
        std::string(" Production:\n") + rendering::resource_to_text(Buildings.at(type).production, "   ");

    std::string upkeep_string =
        std::string(" Upkeep:\n") + rendering::resource_to_text(Buildings.at(type).upkeep, "   ");
    upkeep_string += "   Workers: " + std::to_string((int)Buildings.at(type).workers) + "\n";

    return std::string(Buildings.at(type).description) + "\n" + cost_string + "\n" + production_string + upkeep_string;
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

    sdl_utilities::render_text_center(r.get(), gr->infobox_font, get_building_info(type),
                                      buildings_meta::frame_width / 2, buildings_meta::frame_height / 2,
                                      buildings_meta::frame_width, SDL_Color{0xFF, 0xFF, 0xFF, 0xFF});
}

void rendering::render_ship_info_box(client_context& context, const unsigned int design_id, int x, int y)
{
    auto& r = context.r;
    auto& gr = context.gr;
    auto gs = context.getGameState();

    const auto& d = gs.value->player->ship_designs[design_id];

    sdl_utilities::set_viewport(r.get(), x, y, buildings_meta::frame_width, buildings_meta::frame_height);
    sdl_utilities::paint_background(r.get(), SDL_Color{0x00, 0x00, 0x00, 200});

    sdl_utilities::render_text_center(r.get(), gr->infobox_font, get_ship_info(d), buildings_meta::frame_width / 2,
                                      buildings_meta::frame_height / 2, buildings_meta::frame_width,
                                      SDL_Color{0xFF, 0xFF, 0xFF, 0xFF});
}
