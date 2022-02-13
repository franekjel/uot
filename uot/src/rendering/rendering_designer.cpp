#include "rendering/rendering_designer.h"
#include <array>
#include "client_context.h"
#include "game_resources.h"
#include "input_utilities.h"
#include "msg_queue.h"
#include "player.h"
#include "rendering/rendering_universe.h"

using namespace rendering;

constexpr std::array<std::string_view, 3> hull_names{"Small Hull", "Medium Hull", "Grand Hull"};

view_t render_designer_view::_up() { return std::make_shared<render_universe_view>(); }

view_t render_designer_view::_down(client_context& context) { return std::make_shared<render_designer_view>(); }

void render_designer_view::_mouse_handler(client_context& context, Uint32 event_type, SDL_MouseButtonEvent m, int x,
                                          int y)
{
    namespace iu = input_utilities;
    const auto et = input_utilities::get_designer_event_type(event_type, m, x, y);

    if (et == iu::uot_event_type::designer_left_click_available)
    {
        using AreaType = size_settings::designer_available_area;
        x = x - AreaType::x_offset;
        y = y - AreaType::y_offset;
        modules_available->handle_timed_click(context, x, y);
    }

    if (et == iu::uot_event_type::designer_left_click_chosen)
    {
        using AreaType = size_settings::designer_chosen_area;
        x = x - AreaType::x_offset;
        y = y - AreaType::y_offset;
        modules_chosen->handle_timed_click(context, x, y);
    }

    if (et == iu::uot_event_type::designer_left_click_hull)
    {
        using AreaType = size_settings::designer_hull_area;
        x = x - AreaType::x_offset;
        y = y - AreaType::y_offset;
        hull->handle_timed_click(context, x, y);
    }

    if (et == iu::uot_event_type::designer_left_click_info)
    {
        using AreaType = size_settings::designer_info_area;
        x = x - AreaType::x_offset;
        y = y - AreaType::y_offset;

        const auto& pos = create_design_button->pos;
        if (iu::check_collision(x, y, pos.x, pos.y, pos.w, pos.h))
        {
            // calling a lambda, possibly losing a few instructions
            create_design_button->clicked(context);
        }
    }
}

std::string rendering::render_designer_view::get_design_cost()
{
    std::string cost{"Current costs: \n"};
    bool _h = hull->selected_elem.has_value();
    std::map<Resource, float> disp_c = current_costs;

    if(_h) {
        auto& h = ShipHulls.at(_hull[hull->selected_elem.value()]);
        disp_c += h.cost;
    }
    for (auto& [r, c] : disp_c)
    {
        cost += std::string(resourceNames[static_cast<int>(r)]) + ": " + std::to_string(static_cast<int>(c)) + "\n";
    }

    return cost + "\n";
}

std::string rendering::render_designer_view::get_design_upkeep()
{
    std::string upkeep{"Current upkeep: \n"};

    std::map<Resource, float> disp_upk;

    if (hull->selected_elem.has_value()) {
        auto& h = ShipHulls.at(_hull[hull->selected_elem.value()]);
        disp_upk += h.additional_upkeep;
        disp_upk += ShipDesign::percentage_cost_upkeep * h.cost;
    }

    disp_upk += current_upkeep;


    for (auto& [r, c] : disp_upk)
    {
        upkeep += std::string(resourceNames[static_cast<int>(r)]) + ": " + std::to_string(static_cast<int>(c)) + "\n";
    }

    return upkeep + "\n";
}

std::string rendering::render_designer_view::get_design_worker_cost()
{
    return "Worker weeks: " +
           std::to_string(static_cast<int>(current_worker_weeks +
                                           (hull->selected_elem.has_value()
                                                ? ShipHulls.at(_hull[hull->selected_elem.value()]).worker_weeks_cost
                                                : 0)));
}

void rendering::render_designer_view::render_design_info(client_context& context)
{
    auto& r = context.r;
    auto& gr = context.gr;
    auto& gui = context.gui;

    sdl_utilities::set_render_viewport<size_settings::designer_info_area>(r.get());
    sdl_utilities::paint_background(r.get(), SDL_Color{0x00, 0x00, 0x00, 200});

    std::string name = "New Design";

    sdl_utilities::render_text(r.get(), gr->main_font, name.c_str(), size_settings::designer_info_area::width / 2,
                               fonts::main_font_size / 2 + 30, size_settings::designer_info_area::width - 50,
                               {0xFF, 0xFF, 0xFF, 0xFF});

    sdl_utilities::set_render_viewport<size_settings::designer_info_text_area>(r.get());
    // cost
    auto cost = get_design_cost();
    // upkepp
    auto upkeep = get_design_upkeep();
    // worker cost
    auto wcost = get_design_worker_cost();

    sdl_utilities::render_text(r.get(), gr->secondary_font, cost + upkeep + wcost,
                               size_settings::designer_info_text_area::width / 2,
                               size_settings::designer_info_text_area::height / 2 + info_offset,
                               size_settings::designer_info_text_area::width - 50, {0xFF, 0xFF, 0xFF, 0xFF});

    using AreaType = size_settings::designer_info_area;
    sdl_utilities::set_render_viewport<AreaType>(r.get());
    int x, y;
    SDL_GetMouseState(&x, &y);
    x = x - AreaType::x_offset;
    y = y - AreaType::y_offset;

    const auto& pos = create_design_button->pos;
    auto b_hit = (input_utilities::check_collision(x, y, pos.x, pos.y, pos.w, pos.h));

    create_design_button->draw(context, b_hit);
}

void render_designer_view::_wheel_handler(client_context& context, int x, int y, int xmov, int ymov)
{
    namespace iu = input_utilities;
    const auto et = input_utilities::get_designer_scroll_type(x, y);
    if (et == iu::uot_event_type::designer_scroll_available)
    {
        modules_available->offset = std::clamp(0, static_cast<int>(modules_available->offset + 4 * ymov),
                                               static_cast<int>(modules_available->elems.size() * 80));
    }

    if (et == iu::uot_event_type::designer_scroll_chosen)
    {
        modules_chosen->offset = std::clamp(0, static_cast<int>(modules_chosen->offset + 4 * ymov),
                                            static_cast<int>(modules_chosen->elems.size() * 80));
    }

    if (et == iu::uot_event_type::designer_scroll_info_text)
    {
        info_offset += 4 * ymov;
    }
}

void render_designer_view::key_handler(client_context& context, Uint16 k)
{
    if (k == SDLK_ESCAPE)
    {
        context.gui->current_building.reset();
        context.view = _up();
    }
}

void render_designer_view::_draw(client_context& context)
{
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
    render_list(context, modules_available, false);

    sdl_utilities::set_render_viewport<size_settings::designer_chosen_area>(r.get());
    sdl_utilities::paint_frame(r.get(), SDL_Color{0xFF, 0xFF, 0xFF, 0xFF}, SDL_Color{0x00, 0x00, 0x00, 0xFF});
    render_list(context, modules_chosen, false);

    sdl_utilities::set_render_viewport<size_settings::designer_hull_area>(r.get());
    sdl_utilities::paint_frame(r.get(), SDL_Color{0xFF, 0xFF, 0xFF, 0xFF}, SDL_Color{0x00, 0x00, 0x00, 0xFF});
    render_list(context, hull, false);

    render_design_info(context);
}

void render_designer_view::init(client_context& context)
{
    std::vector<std::string> v_available;
    std::vector<std::string> v_chosen;
    std::vector<std::string> v_hulls;

    auto gs = context.getGameState();
    for (auto& m : gs.value->player->available_modules)
    {
        v_available.push_back(Modules.at(m).name);
        _available.push_back(m);
    }

    for (auto& h : gs.value->player->available_hulls)
    {
        v_hulls.push_back(std::string(hull_names[static_cast<int>(h) - static_cast<int>(ShipHull::SmallShipHull)]));
        _hull.push_back(h);
    }

    modules_available = std::make_shared<ui_list_state>(
        v_available,
        generic_button{[&](client_context& context)
                       {
                           if (modules_available->selected_elem.has_value())
                           {
                               const auto v = modules_available->selected_elem.value();
                               const auto& m = Modules.at(_available[v]);
                               _chosen.push_back(_available[v]);
                               modules_chosen->elems.push_back(modules_available->elems[v]);

                               current_costs += m.cost;
                               current_upkeep += m.additional_upkeep;
                               current_worker_weeks += m.worker_weeks_cost_per_size * m.size;
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
                               const auto v = modules_chosen->selected_elem.value();
                               const auto& m = Modules.at(_chosen[v]);

                               for(const auto& [r, c] : m.cost) {
                                   current_costs[r] -= c;
                               }

                               for(const auto& [r, c] : m.additional_upkeep) {
                                   current_upkeep[r] -= c;
                               }

                               current_worker_weeks -= m.worker_weeks_cost_per_size * m.size;

                               modules_chosen->elems.erase(modules_chosen->elems.cbegin() + v,
                                                           modules_chosen->elems.cbegin() + v + 1);
                               _chosen.erase(_chosen.cbegin() + v, _chosen.cbegin() + v + 1);
                           }
                       },
                       "REMOVE",
                       {size_settings::planet_build_area::width / 2 - 180, 360, 360, 50}},
        300, 50, 50, 10);

    hull = std::make_shared<ui_list_state>(
        v_hulls,
        generic_button{[&](client_context& context) {},
                       "DUMMY",
                       {size_settings::planet_build_area::width / 2 - 180, 360, 360, 50}},
        300, 50, 50, 10);

    create_design_button = std::make_unique<generic_button>(
        generic_button{[&](client_context& context)
                       {
                           static unsigned int id{0u};
                           std::cout << "click" << std::endl;
                           if (hull->selected_elem.has_value())
                           {
                               std::map<ModuleType, int> sides;
                               std::map<ModuleType, int> inside;
                               for (auto e : _chosen)
                               {
                                   if (Modules.at(e).destination == Module::ModuleDestination::Inside)
                                   {
                                       inside[e]++;
                                   }
                                   else if (Modules.at(e).destination == Module::ModuleDestination::Sides)
                                   {
                                       sides[e]++;
                                   }
                               }
                               auto s = std::make_shared<ShipDesign>(id++, "New Design" + std::to_string(id),
                                                                     _hull[hull->selected_elem.value()], sides, inside);

                               auto mq = context.getActionQueue().value;
                               mq->build_design(s, false);
                               _chosen.clear();
                               modules_chosen->elems.clear();
                               hull->selected_elem.reset();
                               current_costs.clear();
                               current_upkeep.clear();
                               current_worker_weeks = 0u;
                           }
                       },
                       "CREATE",
                       {size_settings::designer_info_area::width / 2 - 150, 700, 300, 50}});
}

inline void render_designer_view::refresh_lists(client_context& context)
{
    auto gs = context.getGameState();
    modules_available->elems.clear();
    _available.clear();
    _hull.clear();
    hull->elems.clear();

    for (auto& m : gs.value->player->available_modules)
    {
        modules_available->elems.push_back(Modules.at(m).name);
        _available.push_back(m);
    }

    for (auto& h : gs.value->player->available_hulls)
    {
        hull->elems.push_back(std::string(hull_names[static_cast<int>(h) - static_cast<int>(ShipHull::SmallShipHull)]));
        _hull.push_back(h);
    }
}
