#define _USE_MATH_DEFINES
#include "rendering_sector.h"
#include <cmath>
#include <iomanip>
#include <sstream>

#include "client_context.h"
#include "game_gui.h"
#include "game_resources.h"
#include "game_state.h"
#include "input_utilities.h"
#include "msg_queue.h"
#include "planet.h"
#include "player.h"

void rendering::render_sector_view::_wheel_handler(client_context& context, int x, int y, int xmov, int ymov)
{
    auto& gui = context.gui;
    auto current_fleet = gui->current_fleet;
    if (current_fleet.has_value() && input_utilities::check_view_area_collision<size_settings::fleet_info_area>(x, y))
    {
        gui->current_fleet_info_offset += 4 * ymov;
    }
}

void rendering::render_sector_view::_draw(client_context& context)
{
    render_background(context);
    auto& r = context.r;
    const auto& gr = context.gr;
    auto& gui = context.gui;

    sdl_utilities::set_render_viewport<size_settings::resource_area>(r.get());
    sdl_utilities::paint_background(r.get(), SDL_Color{0x00, 0x00, 0x00, 150});
    render_resource_bar(context);

    // =======================================================
    // draw the left, main game panel
    sdl_utilities::set_render_viewport<size_settings::play_area>(r.get());
    sdl_utilities::paint_frame_textured(r.get(), SDL_Color{0xFF, 0xFF, 0xFF, 0xFF}, gr->sky_texture);
    sdl_utilities::set_render_viewport<size_settings::play_area>(r.get());

    SDL_Rect s{0, 0, gr->sector_boundary.w, gr->sector_boundary.w};
    const auto _w = static_cast<int>(size_settings::play_area::height);
    const auto _h = static_cast<int>(size_settings::play_area::height);
    SDL_Rect d{size_settings::play_area::width / 2 - _w / 2, size_settings::play_area::height / 2 - _h / 2, _w, _h};

    SDL_RenderCopyEx(r.get(), gr->sector_boundary.t.get(), &s, &d, 0.0, NULL, SDL_FLIP_NONE);

    auto current_sector = gui->current_sector;
    if (current_sector.has_value())
    {
        // sector render here
        const auto& curr = current_sector.value();
        for (auto& neighbor : current_sector.value()->neighbors)
        {
            const auto unit = (neighbor->position - current_sector.value()->position).normalized();
            const auto offset = unit * 0.5 * size_settings::play_area::height;

            SDL_Rect s{0, 0, gr->jump_zone.w, gr->jump_zone.w};
            const int _w = gr->jump_zone.w;
            const int _h = gr->jump_zone.h;
            SDL_Rect d{size_settings::play_area::width / 2 + static_cast<int>(offset.x) - _w / 2,
                       size_settings::play_area::height / 2 + static_cast<int>(offset.y) - _h / 2, _w, _h};

            SDL_RenderCopyEx(r.get(), gr->jump_zone.t.get(), &s, &d, 0.0, NULL, SDL_FLIP_NONE);
        }
        render_sector_sector_helper(context, curr);
    }
    render_object_selection(context);

    // =======================================================
    // draw the right, information / GUI panel
    sdl_utilities::set_render_viewport<size_settings::context_area>(r.get());
    sdl_utilities::paint_frame(r.get(), SDL_Color{0xFF, 0xFF, 0xFF, 0xFF}, SDL_Color{0x00, 0x00, 0x00, 0xFF});

    // check inside
    render_selected_object_info(context);
    // check is inside
    render_selected_fleet_info(context);
}

void rendering::render_object_selection(const client_context& context)
{
    const auto& r = context.r;
    const auto& gr = context.gr;
    const auto& gui = context.gui;

    auto current_object = gui->current_object;
    if (!current_object.has_value())
    {
        return;
    }

    const Point pos = current_object.value()->position;
    const int tex_size = planets_meta::texture_size[GAS_GIANT_1];

    render_selection_graphics(context, pos, tex_size);
    const auto x = size_settings::play_area::width / 2 + (size_settings::play_area::height / 2) * pos.x;
    const auto y = size_settings::play_area::height / 2 + (size_settings::play_area::height / 2) * pos.y;
    int textureIdx = gui->GetTextureIndex(current_object.value());
    render_planet_helper(context, 0.8, x, y, gr->planetTextures[textureIdx]);
}

void rendering::render_selected_object_info(const client_context& context)
{
    const auto& r = context.r;
    const auto& gr = context.gr;
    const auto& gui = context.gui;

    auto current_object = gui->current_object;
    if (!current_object.has_value())
    {
        return;
    }

    auto pl = std::dynamic_pointer_cast<Planet>(current_object.value());
    auto io = std::dynamic_pointer_cast<InhabitableObject>(current_object.value());
    auto st = std::dynamic_pointer_cast<Star>(current_object.value());
    std::string type = pl ? "PLANET " : io ? "OBJECT " : "STAR ";
    sdl_utilities::render_text_center(r.get(), gr->main_font, type + std::to_string(current_object.value()->id),
                                      size_settings::context_area::width / 2, fonts::main_font_size / 2 + 30,
                                      size_settings::context_area::width - 50, {0xFF, 0xFF, 0xFF, 0xFF});

    // render planet here again
    int textureIdx = gui->GetTextureIndex(current_object.value());
    render_planet_helper(context, 2.0, size_settings::context_area::width / 2,
                         std::min(250, planets_meta::texture_size[textureIdx] * 2), gr->planetTextures[textureIdx]);

    // render planet info
    std::stringstream info;
    info << std::fixed << std::setprecision(0);

    if (pl)
    {
        if (pl->colony)
        {
            info << "Owner: " + std::to_string(pl->colony->owner->id);
            info << (pl->colony->owner->id == gui->player_id_cache ? " (you)\n" : "\n");
            info << "Population: " << roundf(pl->colony->population) << "\n";
            info << "Soldiers: " << roundf(pl->colony->soldiers) << "\n";
        }

        info << "Features: ";
        if (pl->planetary_features.empty())
            info << "None";

        for (auto& f : pl->planetary_features)
        {
            if (f.second < 0)
                continue;
            info << PlanetaryFeaturesTypes.find(f.first)->second.name << " (" << f.second << "), ";
        }
    }
    else if (io)
    {
        const auto& type_info = InhToNameDesc.find(io->object_type)->second;
        info << "Type: " << type_info.first << "\n";
        info << type_info.second << "\n";
        info << "Base: ";
        if (io->base)
        {
            info << "owned by " + std::to_string(io->base->owner->id);
            info << (io->base->owner->id == gui->player_id_cache ? " (you)\n" : "\n");
        }
        else
            info << "none\n";

        info << "Resources:";
        if (io->resource_deposit.empty())
            info << " none\n";
        else
            info << "\n";
        for (auto& res : io->resource_deposit)
        {
            info << resourceNames[static_cast<int>(res.first)] << " (" << roundf(res.second) << ")\n";
        }
    }
    else if (st)
    {
        const auto& type_info = StarToNameDesc.find(st->star_type)->second;
        info << "The center of this sector\n";
        info << "Type: " << type_info.first << "\n";
        info << type_info.second << "\n";
    }

    sdl_utilities::render_text_center(r.get(), gr->secondary_font, info.str(), size_settings::context_area::width / 2,
                                      size_settings::context_area::height * 0.75,
                                      size_settings::context_area::width - 50, {0xFF, 0xFF, 0xFF, 0xFF});
}

void rendering::render_sector_sector_helper(client_context& context, const std::shared_ptr<Sector>& sector)
{
    const auto& gr = context.gr;
    const auto& gui = context.gui;
    for (const auto& [id, p] : sector->objects)
    {
        const auto planet_x =
            size_settings::play_area::width / 2 + (size_settings::play_area::height / 2) * p->position.x;
        const auto planet_y =
            size_settings::play_area::height / 2 + (size_settings::play_area::height / 2) * p->position.y;

        // always the same rec size
        int textureIdx = gui->GetTextureIndex(p);
        render_planet_helper(context, 0.8, planet_x, planet_y, gr->planetTextures[textureIdx]);

        auto pl = std::dynamic_pointer_cast<Planet>(p);
        auto io = std::dynamic_pointer_cast<InhabitableObject>(p);
        if (pl)
        {
            const unsigned int player_id = (pl->colony && pl->colony->owner) ? pl->colony->owner->id : 0;
            render_planet_owner(context, player_id, 0.8, planet_x, planet_y, gr->planetTextures[textureIdx]);
        }
        else if (io)
        {
            const unsigned int player_id = (io->base && io->base->owner) ? io->base->owner->id : 0;
            render_planet_owner(context, player_id, 0.8, planet_x, planet_y, gr->planetTextures[textureIdx]);
        }
    }

    const auto& gs = context.getGameState().value;

    std::unique_lock pr_fleet(gs->present_fleet_mutex);
    for (auto& [id, f] : sector->present_fleets)
    {
        render_fleet(context, f);
    }
    pr_fleet.unlock();

    std::unique_lock en_fleet(gs->enemy_fleet_mutex);
    for (auto& [id, f] : gs->enemies_fleet_in_current_sector)
    {
        render_fleet(context, f);
    }
    en_fleet.unlock();

    render_animations(context);
}

rendering::view_t rendering::render_sector_view::_up() { return std::make_shared<render_universe_view>(); }

rendering::view_t rendering::render_sector_view::_down(client_context& context)
{
    auto ret = std::make_shared<render_planet_view>();
    ret->init(context);
    return ret;
}

void rendering::render_sector_view::_mouse_handler(client_context& context, Uint32 event_type, SDL_MouseButtonEvent m,
                                                   int x, int y)
{
    auto& gui = context.gui;
    const auto& gr = context.gr;
    namespace iu = input_utilities;
    const auto et = input_utilities::get_event_type(event_type, m, x, y);

    using AreaType = size_settings::play_area;
    auto& current_sector = context.gui->current_sector;
    auto& current_object = context.gui->current_object;
    auto& current_fleet = context.gui->current_fleet;

    if (et == input_utilities::uot_event_type::left_click_context ||
        et == input_utilities::uot_event_type::motion_context)
    {
        x = x - size_settings::context_area::x_offset;
        y = y - size_settings::context_area::y_offset;
    }
    else
    {
        x = x - AreaType::x_offset;
        y = y - AreaType::y_offset;
    }

    if (et == iu::uot_event_type::left_click_play)
    {
        const auto& curr = current_sector.value();
        gui->current_fleet_info_offset = 0;

        const unsigned int player_id = context.getGameState().value->player->id;
        for (const auto& [id, fleet] : current_sector.value()->present_fleets)
        {
            if (fleet->owner_id != player_id)
                continue;
            const auto fleet_x =
                size_settings::play_area::width / 2 + (size_settings::play_area::height / 2) * fleet->position.x;
            const auto fleet_y =
                size_settings::play_area::height / 2 + (size_settings::play_area::height / 2) * fleet->position.y;
            if (iu::check_collision_circle(x, y, fleet_x, fleet_y, 16))  // 16 -magic number size of fleet sprite
            {
                current_fleet = fleet;
                current_object.reset();
                return;
            }
        }
        current_fleet.reset();

        for (const auto& [id, sec_obj] : curr->objects)
        {
            const auto tex_size = planets_meta::texture_size[GAS_GIANT_1];
            const auto planet_x =
                size_settings::play_area::width / 2 + (size_settings::play_area::height / 2) * sec_obj->position.x;
            const auto planet_y =
                size_settings::play_area::height / 2 + (size_settings::play_area::height / 2) * sec_obj->position.y;

            if (iu::check_collision(x, y, planet_x - 0.5 * tex_size, planet_y - 0.5 * tex_size, tex_size, tex_size))
            {
                auto c_obj = gui->current_object;
                if (c_obj.has_value() && c_obj.value()->position == sec_obj->position &&
                    dynamic_cast<Planet*>(c_obj.value().get()))
                {
                    auto p = dynamic_cast<Planet*>(c_obj.value().get());
                    if (p->colony && p->colony->owner->id == context.player_id)
                    {
                        context.view = down(context);
                        current_object.reset();
                    }
                }
                current_object = sec_obj;
                return;
            }
        }
        current_object.reset();

        for (const auto& neighbor : current_sector.value()->neighbors)
        {
            const auto unit = (neighbor->position - current_sector.value()->position).normalized();
            const auto offset = unit * 0.5 * size_settings::play_area::height;

            const auto _w = gr->jump_zone.w;
            const auto _h = gr->jump_zone.h;
            const auto _x = size_settings::play_area::width / 2 + static_cast<int>(offset.x) - _w / 2;
            const auto _y = size_settings::play_area::height / 2 + static_cast<int>(offset.y) - _h / 2;

            if (iu::check_collision(x, y, _x, _y, _w, _h))
            {
                gui->reset_selection();
                current_sector = neighbor;
            }
        }
    }
    else if (et == iu::uot_event_type::right_click_play)
    {
        auto c_flt = gui->current_fleet;
        if (c_flt.has_value())
        {
            auto& f = c_flt.value();

            Point pos{(x - size_settings::play_area::width / 2.0f) / (size_settings::play_area::height / 2.0f),
                      (y - size_settings::play_area::height / 2.0f) / (size_settings::play_area::height / 2.0f)};

            if (pos.x * pos.x + pos.y * pos.y > 1.0f)
                return;
            context.getActionQueue().value->request_fleet_move(f->id, pos);
            f->wanted_position = pos;
        }
    }
    else if (et == iu::uot_event_type::left_click_context || et == iu::uot_event_type::motion_context)
    {
        bool hit = false;

        for (auto& b : gui->selected_fleet_buttons)
        {
            std::visit(
                [&](auto&& v)
                {
                    const auto& pos = v->pos;
                    if (iu::check_collision(x, y, pos.x, pos.y, pos.w, pos.h))
                    {
                        if (event_type == SDL_MOUSEBUTTONDOWN && v->is_active())
                        {
                            v->clicked(context);
                            return;
                        }

                        context.gui->focused_button = v->button_id;
                        hit = true;
                        return;
                    }
                },
                b);
        }

        if (!hit)
            context.gui->focused_button.reset();
    }
}

void rendering::render_sector_view::key_handler(client_context& context, Uint16 k)
{
    if (k == SDLK_ESCAPE)
    {
        context.gui->reset_selection();
        context.view = _up();
    }
}

void rendering::render_fleet(const client_context& context, const std::shared_ptr<Fleet> f)
{
    const long dt = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() -
                                                                          context.gui->last_turn_time)
                        .count();

    const Point vec = std::isnan(f->wanted_position.x) ? Point{0, 0} : f->wanted_position - f->position;
    Point v = f->position + f->movement_vec * (float(dt) / float(turn_time_ms));

    auto current_fleet = context.gui->current_fleet;
    if (current_fleet.has_value() && f == current_fleet.value())
    {
        render_fleet_weapon_ranges(context, v, f);
        render_selection_graphics(context, v, 32);
    }

    const auto fleet_x = size_settings::play_area::width / 2 + (size_settings::play_area::height / 2) * v.x;
    const auto fleet_y = size_settings::play_area::height / 2 + (size_settings::play_area::height / 2) * v.y;

    const int w = context.gr->fleet_textures[f->owner_id].w;
    const int h = context.gr->fleet_textures[f->owner_id].h;

    const SDL_Rect d = {static_cast<int>(fleet_x - 0.5 * w), static_cast<int>(fleet_y - 0.5 * h), w, h};

    const float angle = std::atan2(vec.y, vec.x) + M_PI_2;

    SDL_RenderCopyEx(context.r.get(), context.gr->fleet_textures[f->owner_id].t.get(), nullptr, &d,
                     angle * 360.0f / (2.0f * M_PI), nullptr, SDL_FLIP_NONE);
}

void rendering::render_selection_graphics(const client_context& context, const Point pos, const int tex_size)
{
    const auto& r = context.r;
    const auto& gr = context.gr;

    sdl_utilities::set_custom_viewport<size_settings::play_area, size_settings::frame_size>(r.get());
    sdl_utilities::paint_background(r.get(), SDL_Color{0x00, 0x00, 0x00, 150});
    sdl_utilities::set_render_viewport<size_settings::play_area>(r.get());

    const auto x = size_settings::play_area::width / 2 + (size_settings::play_area::height / 2) * pos.x;
    const auto y = size_settings::play_area::height / 2 + (size_settings::play_area::height / 2) * pos.y;

    SDL_Rect s{0, 0, selection_meta::texture_width, selection_meta::texture_height};

    SDL_Rect d{static_cast<int>(x - 0.5 * tex_size), static_cast<int>(y - 0.5 * tex_size), tex_size, tex_size};

    SDL_RenderCopyEx(r.get(), gr->selectionTextures[selection_types::SECTOR_SELECTION].get(), &s, &d,
                     SDL_GetTicks() / 100, NULL, SDL_FLIP_NONE);
}

void rendering::render_selected_fleet_info(client_context& context)
{
    const auto& r = context.r;
    const auto& gr = context.gr;
    const auto& gui = context.gui;

    auto current_fleet = gui->current_fleet;
    if (!current_fleet.has_value())
    {
        return;
    }

    const auto& f = current_fleet.value();

    sdl_utilities::render_text_center(r.get(), gr->main_font, "fleet " + std::to_string(f->id),
                                      size_settings::context_area::width / 2, fonts::main_font_size / 2 + 30,
                                      size_settings::context_area::width - 50, {0xFF, 0xFF, 0xFF, 0xFF});

    std::string fleet_info = "";

    fleet_info += "Action: " + std::string(Fleet::kActionNames[f->current_action]) + "\n";
    fleet_info += "HP: " + std::to_string(int(f->current_hp)) + "/" + std::to_string(int(f->max_hp)) + "\n";
    fleet_info +=
        "shields: " + std::to_string(int(f->current_shields)) + "/" + std::to_string(int(f->max_shields)) + "\n";
    fleet_info += "avg energy: " + std::to_string(int(f->average_energy * 100.0f)) + "%\n";

    std::stringstream ss;
    ss << "civilians: " << std::fixed << std::setprecision(0) << f->civilians << "\n";
    ss << "soldiers: " << std::fixed << std::setprecision(0) << f->soldiers << "\n";
    ss << "places left: " << std::fixed << std::setprecision(0) << (f->human_capacity - (f->civilians + f->soldiers))
       << "\n";

    fleet_info += ss.str();

    fleet_info += "ships:\n";
    for (const auto& [id, s] : f->ships)
    {
        fleet_info += " id" + std::to_string(id) + ": " + s->design->name + " en:" + std::to_string(int(s->energy)) +
                      "/" + std::to_string(int(s->max_energy)) + "\n    hp:" + std::to_string(int(s->hp)) + "/" +
                      std::to_string(int(s->max_hp)) + " sh:" + std::to_string(int(s->shield)) + "/" +
                      std::to_string(int(s->max_shield)) + "\n";
    }

    sdl_utilities::set_render_viewport<size_settings::fleet_info_area>(r.get());
    sdl_utilities::render_text_top_center(r.get(), gr->infobox_font, fleet_info, size_settings::context_area::width / 2,
                                          fonts::infobox_font_size / 2 + gui->current_fleet_info_offset,
                                          size_settings::context_area::width - 50, {0xFF, 0xFF, 0xFF, 0xFF});

    sdl_utilities::set_render_viewport<size_settings::context_area>(r.get());
    gui->set_button_font(gr->action_button_font);
    for (auto& b : gui->selected_fleet_buttons)
    {
        set_fleet_button_color(b, context);
        std::visit([&](auto&& v) { v->draw(context, v->button_id == gui->focused_button); }, b);
    }
    gui->reset_button_font();
}

void rendering::render_fleet_weapon_ranges(const client_context& context, const Point pos,
                                           const std::shared_ptr<Fleet> f)
{
    const auto& r = context.r;
    const auto& gr = context.gr;
    const auto x = size_settings::play_area::width / 2 + (size_settings::play_area::height / 2) * pos.x;
    const auto y = size_settings::play_area::height / 2 + (size_settings::play_area::height / 2) * pos.y;

    for (const auto& [t, c] : f->fleet_weapons)
    {
        const auto& tex = gr->circle_textures.at(t);
        const SDL_Rect s{0, 0, tex.w, tex.h};
        const SDL_Rect d{static_cast<int>(x - 0.5 * tex.w), static_cast<int>(y - 0.5 * tex.h), tex.w, tex.h};
        SDL_RenderCopyEx(r.get(), tex.t.get(), &s, &d, 0, nullptr, SDL_FLIP_NONE);
    }
}

void rendering::render_animations(const client_context& context)
{
    for (const auto& [id, a] : context.gui->fight_animations)
    {
        a->Render(context.r);
    }
}
