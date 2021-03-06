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

    if (et == iu::uot_event_type::designer_motion_available)
    {
        using AreaType = size_settings::designer_available_area;
        int _x = x - AreaType::x_offset;
        int _y = y - AreaType::y_offset;

        auto res = modules_available->handle_motion(_x, _y);
        if (res.has_value())
        {
            box.x = x;
            box.y = y;
            box.module_type = _available[res.value()];
            box.hull_type.reset();
            return;
        }
        box.reset();
    }

    if (et == iu::uot_event_type::designer_motion_chosen)
    {
        using AreaType = size_settings::designer_chosen_area;
        int _x = x - AreaType::x_offset;
        int _y = y - AreaType::y_offset;

        auto res = modules_chosen->handle_motion(_x, _y);
        if (res.has_value())
        {
            box.x = x;
            box.y = y;
            box.module_type = _chosen[res.value()];
            box.hull_type.reset();
            return;
        }
        box.reset();
    }

    if (et == iu::uot_event_type::designer_motion_hull)
    {
        using AreaType = size_settings::designer_hull_area;
        int _x = x - AreaType::x_offset;
        int _y = y - AreaType::y_offset;

        auto res = hull->handle_motion(_x, _y);
        if (res.has_value())
        {
            box.x = x;
            box.y = y;
            box.module_type.reset();
            box.hull_type = _hull[res.value()];
            return;
        }
        box.reset();
    }
}

std::string rendering::render_designer_view::get_general_info()
{
    auto _h = hull->selected_elem.has_value();
    general_info disp_info = sh_info;

    if (_h)
    {
        const auto& h = ShipHulls.at(_hull[hull->selected_elem.value()]);
        disp_info.hp += h.hp;
        disp_info.energy_use_normal += h.engines_energy_consumtion;
        disp_info.energy_use_max += h.engines_energy_consumtion;
        disp_info.speed = h.speed;
        disp_info.ship_aggro = h.ship_aggro;
        disp_info.space_inside = h.inside_size;
        disp_info.space_sides = h.sides_size;
    }

    auto ret = std::string("Current design: \n");
    ret += "Free space inside: " + std::to_string(disp_info.taken_inside) + "/" +
           std::to_string(disp_info.space_inside) + "\n";
    ret += "Free space sides: " + std::to_string(disp_info.taken_sides) + "/" + std::to_string(disp_info.space_sides) +
           "\n";
    ret += "HP: " + std::to_string(disp_info.hp) + "\n";
    ret += disp_info.hp_regen == 0 ? "" : "HP regeneration: " + std::to_string(disp_info.hp_regen) + "\n";
    ret += "Max Energy: " + std::to_string(disp_info.max_energy) + "\n";
    ret += "Energy use: " + std::to_string(disp_info.energy_use_normal) + "\n";
    ret += "Max energy use: " + std::to_string(disp_info.energy_use_max) + "\n";
    ret += "Energy regeneration: " + std::to_string(disp_info.energy_regen) + "\n";
    ret += disp_info.shields == 0 ? "" : "Shields: " + std::to_string(disp_info.shields) + "\n";
    ret += disp_info.human_capacity == 0 ? "" : "Human Capacity: " + std::to_string(disp_info.human_capacity) + "\n";
    ret += disp_info.construction == 0 ? "" : "Construction Points: " + std::to_string(disp_info.construction) + "\n";
    ret += disp_info.attack == 0 ? "" : "Attack count: " + std::to_string(disp_info.attack) + "\n";
    ret += "Speed: " + std::to_string(disp_info.speed) + "\n";
    ret += "Aggro: " + std::to_string(disp_info.ship_aggro) + "\n";
    return ret;
}

std::string rendering::render_designer_view::get_design_cost()
{
    bool _h = hull->selected_elem.has_value();
    std::map<Resource, float> cost = current_costs;

    if (_h)
    {
        auto& h = ShipHulls.at(_hull[hull->selected_elem.value()]);
        cost += h.cost;
    }

    return std::string("Costs:\n") + resource_to_text(cost) + "\n";
}

std::string rendering::render_designer_view::get_design_upkeep()
{
    std::map<Resource, float> upkeep;

    if (hull->selected_elem.has_value())
    {
        auto& h = ShipHulls.at(_hull[hull->selected_elem.value()]);
        upkeep += h.additional_upkeep;
        upkeep += ShipDesign::percentage_cost_upkeep * h.cost;
    }
    upkeep += current_upkeep;
    return std::string("Upkeep:\n") + resource_to_text(upkeep) + "\n";
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

    SDL_Rect s{0, 0, cabin_meta::w, cabin_meta::h};
    constexpr float scale = size_settings::designer_info_area::width / static_cast<float>(cabin_meta::w);
    SDL_Rect d{0, 0, size_settings::designer_info_area::width, static_cast<int>(scale * cabin_meta::h)};

    SDL_RenderCopyEx(r.get(), gr->cabin_texture.get(), &s, &d, 0., NULL, SDL_FLIP_NONE);

    sdl_utilities::paint_background(r.get(), SDL_Color{0x00, 0x00, 0x00, 200});

    std::string name = "New Design";

    sdl_utilities::render_text_center(r.get(), gr->main_font, name.c_str(),
                                      size_settings::designer_info_area::width / 2, fonts::main_font_size / 2 + 30,
                                      size_settings::designer_info_area::width - 50, {0xFF, 0xFF, 0xFF, 0xFF});

    sdl_utilities::set_render_viewport<size_settings::designer_info_text_area>(r.get());
    auto general_info = get_general_info();
    // cost
    auto cost = get_design_cost();
    // upkepp
    auto upkeep = get_design_upkeep();
    // worker cost
    auto wcost = get_design_worker_cost();

    sdl_utilities::render_text_top_center(r.get(), gr->secondary_font, general_info + "\n" + cost + upkeep + wcost,
                                          size_settings::designer_info_text_area::width / 2 + 80,
                                          size_settings::designer_info_text_area::y_offset + info_offset,
                                          size_settings::designer_info_text_area::width - 120,
                                          {0xFF, 0xFF, 0xFF, 0xFF});

    using AreaType = size_settings::designer_info_area;
    sdl_utilities::set_render_viewport<AreaType>(r.get());
    int x, y;
    SDL_GetMouseState(&x, &y);
    x = x - AreaType::x_offset;
    y = y - AreaType::y_offset;

    if (cur_design.has_value())
    {
        const auto& pos = create_design_button->pos;
        auto b_hit = (input_utilities::check_collision(x, y, pos.x, pos.y, pos.w, pos.h));

        create_design_button->draw(context, b_hit);
    }
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

inline std::string get_module_info(ModuleType type)
{
    const auto& m = Modules.at(type);
    std::string i = m.name + "\n\n";
    i += "Size: " + std::to_string(m.size) + "\n";
    i += "Destination: " + (m.destination == Module::Sides ? std::string("sides") : std::string("inside")) + "\n";
    if (m.generating_energy > 0)
    {
        i += "Energy generation: " + rendering::f2s(m.generating_energy) + "\n";
        i += "Energy capacity: " + rendering::f2s(m.energy_capacity) + "\n";
    }
    else
        i += "Energy usage: " + rendering::f2s(m.energy_usage) + "\n";
    if (m.shield_capacity > 0)
        i += "Shield capacity: " + rendering::f2s(m.shield_capacity) + "\n";
    if (m.additional_hp > 0)
        i += "Additional HP: " + rendering::f2s(m.additional_hp) + "\n";
    if (type == ModuleType::NanobotsSelfRepairModule)
        i += "HP regeneration: " + rendering::f2s(Module::nanobots_hp_regen_amount) + "\n";
    if (m.contruction_speed > 0)
        i += "Contruction speed: " + rendering::f2s(m.contruction_speed) + "\n";
    if (m.human_capacity > 0)
        i += "Human capacity: " + std::to_string(int(m.human_capacity)) + "\n";

    if (m.weapon.has_value())
    {
        const auto& w = m.weapon.value();
        i += "Attacks: " + std::to_string(w.attack_count) + "\n";
        i += "Damage: " + rendering::f2s(w.damage) + "\n";
        i += "Range: " + rendering::f2s(w.range) + "\n";

        switch (w.special_features)
        {
            case Weapon::BypassShield:
                i += "Bypass shields\n";
                break;
            case Weapon::ShieldDamageBonus:
                i += "Shield damage bonus\n";
                break;
            case Weapon::HPDamageBonus:
                i += "HP damage bonus\n";
                break;
            default:
                break;
        }
    }
    i += "\nCost:\n";
    i += rendering::resource_to_text(m.cost);
    i += "  work:" + rendering::f2s(m.worker_weeks_cost_per_size);
    i += "\nAdditional upkeep:\n";
    i += rendering::resource_to_text(m.additional_upkeep);
    return i;
}

inline std::string get_hull_info(ShipHull::Type type)
{
    const auto& m = ShipHulls.at(type);

    std::string i = hull_type_to_design_name.at(type) + " hull\n\n";
    i += "Sides size: " + std::to_string(m.sides_size) + "\n";
    i += "Inside size: " + std::to_string(m.inside_size) + "\n";
    i += "Base HP: " + rendering::f2s(m.hp) + "\n";
    i += "Speed: " + rendering::f2s(m.speed) + "\n";
    i += "Energy consumption: " + rendering::f2s(m.engines_energy_consumtion) + "\n";
    i += "Required crew: " + rendering::f2s(m.crew) + "\n";
    i += "Warp drive energy: " + rendering::f2s(m.warp_drive_energy) + "\n";
    i += "Aggro: " + rendering::f2s(m.ship_aggro) + "\n";
    i += "\nCost:\n";
    i += rendering::resource_to_text(m.cost);
    i += "  work:" + rendering::f2s(m.worker_weeks_cost);
    i += "\nAdditional upkeep:\n";
    i += rendering::resource_to_text(m.additional_upkeep);

    return i;
}

void rendering::render_module_info_box(client_context& context, ModuleType type, int x, int y)
{
    auto& r = context.r;
    auto& gr = context.gr;

    const auto& m = Modules.at(type);
    sdl_utilities::set_viewport(r.get(), x, y, buildings_meta::frame_width, buildings_meta::frame_height);
    sdl_utilities::paint_background(r.get(), SDL_Color{0x00, 0x00, 0x00, 200});

    sdl_utilities::render_text_center(r.get(), gr->infobox_font, get_module_info(type), buildings_meta::frame_width / 2,
                                      buildings_meta::frame_height / 2, buildings_meta::frame_width,
                                      SDL_Color{0xFF, 0xFF, 0xFF, 0xFF});
}

void rendering::render_hull_info_box(client_context& context, ShipHull::Type type, int x, int y)
{
    auto& r = context.r;
    auto& gr = context.gr;

    const auto& h = ShipHulls.at(type);
    sdl_utilities::set_viewport(r.get(), x, y, buildings_meta::frame_width, buildings_meta::frame_height);
    sdl_utilities::paint_background(r.get(), SDL_Color{0x00, 0x00, 0x00, 200});

    sdl_utilities::render_text_center(r.get(), gr->infobox_font, get_hull_info(type), buildings_meta::frame_width / 2,
                                      buildings_meta::frame_height / 2, buildings_meta::frame_width,
                                      SDL_Color{0xFF, 0xFF, 0xFF, 0xFF});
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

    if (box.module_type.has_value())
    {
        rendering::render_module_info_box(context, box.module_type.value(), box.x, box.y);
    }
    else if (box.hull_type.has_value())
    {
        rendering::render_hull_info_box(context, box.hull_type.value(), box.x, box.y);
    }
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

                               if (m.destination == Module::ModuleDestination::Inside)
                               {
                                   sh_info.taken_inside += m.size;
                               }
                               else
                               {
                                   sh_info.taken_sides += m.size;
                               }
                               sh_info.hp += m.additional_hp;
                               sh_info.max_energy += m.energy_capacity;
                               sh_info.energy_regen += m.generating_energy;
                               sh_info.energy_use_normal += m.energy_usage;
                               sh_info.energy_use_max += m.energy_usage;
                               sh_info.shields += m.shield_capacity;
                               sh_info.human_capacity = m.human_capacity;
                               sh_info.hp_regen = _available[v] == ModuleType::NanobotsSelfRepairModule
                                                      ? Module::nanobots_hp_regen_amount
                                                      : 0;

                               sh_info.construction += m.contruction_speed;
                               if (m.weapon.has_value())
                               {
                                   auto& w = m.weapon.value();
                                   sh_info.attack += w.attack_count;
                               }
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

                               if (m.destination == Module::ModuleDestination::Inside)
                               {
                                   sh_info.taken_inside -= m.size;
                               }
                               else
                               {
                                   sh_info.taken_sides -= m.size;
                               }
                               sh_info.hp -= m.additional_hp;
                               sh_info.max_energy -= m.energy_capacity;
                               sh_info.energy_regen -= m.generating_energy;
                               sh_info.energy_use_normal -= m.energy_usage;
                               sh_info.energy_use_max -= m.energy_usage;
                               sh_info.shields -= m.shield_capacity;
                               sh_info.human_capacity = m.human_capacity;
                               sh_info.hp_regen = _available[v] == ModuleType::NanobotsSelfRepairModule
                                                      ? Module::nanobots_hp_regen_amount
                                                      : 0;

                               sh_info.construction -= m.contruction_speed;
                               if (m.weapon.has_value())
                               {
                                   auto& w = m.weapon.value();
                                   sh_info.attack -= w.attack_count;
                               }

                               for (const auto& [r, c] : m.cost)
                               {
                                   current_costs[r] -= c;
                               }

                               for (const auto& [r, c] : m.additional_upkeep)
                               {
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
                           if (cur_design.has_value())
                           {
                               unsigned int& id = context.gui->current_design_id;
                               auto s = std::make_shared<ShipDesign>(
                                   id, hull_type_to_design_name.at(cur_design->hull_type) + std::to_string(id),
                                   cur_design->hull_type, std::move(cur_design->sides), std::move(cur_design->inside));
                               id++;
                               cur_design.reset();
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

    update_current_design();
}

void render_designer_view::update_current_design()
{
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
        if (IsShipDesignPossible(_hull[hull->selected_elem.value()], sh_info.taken_sides, sh_info.taken_inside) &&
            IsShipDesignCorrect(_hull[hull->selected_elem.value()], sides, inside))
            cur_design = ShipDesign(0, "NOTHING", _hull[hull->selected_elem.value()], sides, inside);
        else
            cur_design.reset();
    }
    else
        cur_design.reset();
}
