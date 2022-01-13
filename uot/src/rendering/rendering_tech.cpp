#include "rendering/rendering_tech.h"
#include "rendering/rendering_universe.h"
#include "client_context.h"
#include "game_gui.h"
#include "game_resources.h"
#include "game_state.h"
#include "player.h"
#include "utilities/input_utilities.h"
#include <deque>

void rendering::render_tech_view::_wheel_handler(client_context& context, int x, int y, int xmov, int ymov) {}

static int get_children_size(std::map<Technology::TechnologyType, int>& sizes, Technology::TechnologyType type)
{
    auto it = Technologies.find(type);
    if (it->second.unlock.empty())
    {
        sizes.emplace(type, 1);
        return 1;
    }
    
    int size = 0;
    for (auto& t : it->second.unlock)
    {
        size += get_children_size(sizes, t);
    }

    sizes.emplace(type, size);
    return size;
}

static int render_subtree_helper(const client_context& context, std::shared_ptr<game_state> gs,
                                 tech_pos pos, Technology::TechnologyType type)
{
    rendering::render_tech_node(context, gs, pos, type);

    SDL_Point line_start = {(pos.x + 1) * size_settings::tech_node_size::width,
                            pos.y * size_settings::tech_node_size::height + size_settings::tech_node_size::height / 2};

    pos.x++;
    for (auto& tt : Technologies.find(type)->second.unlock)
    {
        SDL_Point line_end = {line_start.x,
                            pos.y * size_settings::tech_node_size::height + size_settings::tech_node_size::height / 2};
        SDL_SetRenderDrawColor(context.r.get(), 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderDrawLine(context.r.get(), line_end.x, line_end.y,
                           line_end.x + size_settings::tech_node_size::x_offset, line_end.y);
        SDL_RenderDrawLine(context.r.get(), line_start.x, line_start.y, line_end.x, line_end.y);
        pos.y = render_subtree_helper(context, gs, pos, tt);
    }

    if(Technologies.find(type)->second.unlock.empty()) pos.y++;
    else SDL_RenderDrawLine(context.r.get(), line_start.x - size_settings::tech_node_size::x_offset,
                            line_start.y, line_start.x, line_start.y);
    return pos.y;
}

void rendering::render_tech_tree(const client_context& context, std::shared_ptr<game_state> gs)
{
    auto offset = render_subtree_helper(context, gs, {0, 0}, Technology::TechnologyType::Engineering);
    render_subtree_helper(context, gs, {0, offset}, Technology::TechnologyType::Spaceships);
}
// void rendering::render_tech_info(const client_context& context, Technology& tech)
// {
//     auto& r = context.r;
//     auto& gr = context.gr;
//     auto& gui = context.gui;



//     auto sector_id = gui->current_sector.value()->sector_id % 10 + 1;

//     sdl_utilities::render_text(r.get(), gr->main_font, "SECTOR NAME", size_settings::context_area::width / 2,
//                                fonts::main_font_size / 2 + 30, size_settings::context_area::width - 50,
//                                {0xFF, 0xFF, 0xFF, 0xFF});

//     // render planet here again
//     render_planet_helper(context, 2.0, size_settings::context_area::width / 2,
//                          std::min(250, planets_meta::texture_size[sector_id] * 2), gr->planetTextures[sector_id]);

//     // render planet info
//     sdl_utilities::render_text(r.get(), gr->secondary_font,
//                                " sector index: " + std::to_string(gui->current_sector.value()->sector_id) +
//                                    "\n planet info 1\n planet info 2\n planet info 3",
//                                size_settings::context_area::width / 2, size_settings::context_area::height * 0.75,
//                                size_settings::context_area::width - 50, {0xFF, 0xFF, 0xFF, 0xFF});
// }

void rendering::render_tech_view::_draw(client_context& context)
{
    SDL_RenderSetLogicalSize(context.r.get(),
                             1900,
                             1080);
    render_background(context);
    // draw the above astronaut buttons
    auto& gr = context.gr;
    auto& r = context.r;
    auto& gui = context.gui;

    sdl_utilities::set_render_viewport<size_settings::resource_area>(r.get());
    sdl_utilities::paint_background(r.get(), SDL_Color{0x00, 0x00, 0x00, 150});
    render_resource_bar(context);

    auto state = context.getGameState();
    auto& gs = state.value;

    // draw the left, main game panel
    sdl_utilities::set_render_viewport<size_settings::play_area>(r.get());
    sdl_utilities::paint_frame_textured(r.get(), SDL_Color{0xFF, 0xFF, 0xFF, 0xFF}, gr->sky_texture);
    sdl_utilities::set_render_viewport<size_settings::tech_area>(r.get());
    if (gs->player)
    {
        render_tech_node(context, gs, {0,0}, Technology::TechnologyType::Engineering);
        render_tech_tree(context, gs);
    }
    // =======================================================
    // draw the right, information / GUI panel
    sdl_utilities::set_render_viewport<size_settings::context_area>(r.get());
    sdl_utilities::paint_frame(r.get(), SDL_Color{0xFF, 0xFF, 0xFF, 0xFF}, SDL_Color{0x00, 0x00, 0x00, 0xFF});

    if (gui->current_sector.has_value())
    {
        rendering::render_tech_info(context);
    }
    else
    {
        for (auto& elem : context.gui->navigation_menu_buttons)
        {
            std::visit([&](auto&& v) { v->draw(context, context.gui->focused_button == v->button_id); }, elem);
        }
    }
}

void rendering::render_tech_info(const client_context& context)
{
    static const auto tile_x = size_settings::play_area::width / generation_meta::sqrt_num_sectors;
    static const auto tile_y = size_settings::play_area::height / generation_meta::sqrt_num_sectors;

    auto& r = context.r;
    auto& gr = context.gr;
    auto& gui = context.gui;

    if (!gui->current_sector.has_value())
    {
        return;
    }

    sdl_utilities::set_custom_viewport<size_settings::play_area, size_settings::frame_size>(r.get());
    sdl_utilities::paint_background(r.get(), SDL_Color{0x00, 0x00, 0x00, 150});
    sdl_utilities::set_render_viewport<size_settings::play_area>(r.get());

    const auto curr = gui->current_sector.value();
    const int tex_size = planets_meta::texture_size[SECTOR_1] * planets_meta::sector_multiplier;
    const int x = size_settings::play_area::width * (0.5f * curr->position.x + 0.5f) - tex_size / 2;
    const int y = size_settings::play_area::height * (0.5f * curr->position.y + 0.5f) - tex_size / 2;
    SDL_Rect s{0, 0, selection_meta::texture_width, selection_meta::texture_height};

    SDL_Rect d{x, y, tex_size, tex_size};

    SDL_RenderCopyEx(r.get(), gr->selectionTextures[selection_types::SECTOR_SELECTION].get(), &s, &d,
                     SDL_GetTicks() / 100, NULL, SDL_FLIP_NONE);
    // TODO : render tech info
}

rendering::view_t rendering::render_tech_view::_up() { return std::make_shared<rendering::render_universe_view>(); }

rendering::view_t rendering::render_tech_view::_down(client_context& context)
{
    return std::make_shared<render_sector_view>();
}

void rendering::render_tech_view::_mouse_handler(client_context& context, Uint32 event_type, SDL_MouseButtonEvent m,
                                                     int x, int y)
{
    namespace iu = input_utilities;
    const auto et = iu::get_event_type(event_type, m, x, y);
    if (et == iu::uot_event_type::left_click_play)
    {
        using AreaType = size_settings::play_area;
        auto state = context.getGameState();
        auto& gs = state.value;
        auto& gr = context.gr;
        auto& current_sector = context.gui->current_sector;
        if (!gs->player)
        {
            return;
        }
        x = x - AreaType::x_offset;
        y = y - AreaType::y_offset;
        const int sprite_off = planets_meta::texture_size[SECTOR_1] * planets_meta::sector_multiplier * 0.5f;
        for (const auto s : gs->player->known_galaxy->sectors)
        {
            if (iu::check_collision(x, y, size_settings::play_area::width * (0.5f + 0.5f * s->position.x) - sprite_off,
                                    size_settings::play_area::height * (0.5f + 0.5f * s->position.y) - sprite_off,
                                    planets_meta::texture_size[SECTOR_1] * planets_meta::sector_multiplier,
                                    planets_meta::texture_size[SECTOR_1] * planets_meta::sector_multiplier))
            {
                if (current_sector.has_value() && current_sector.value()->sector_id == s->sector_id)
                {
                    context.view = _down(context);
                }
                current_sector = s;
                return;
            }
        }
        current_sector.reset();
    }
    else if (et == iu::uot_event_type::left_click_context || et == iu::uot_event_type::motion_context)
    {
        x = x - size_settings::context_area::x_offset;
        y = y - size_settings::context_area::y_offset;

        bool hit = false;
        for (const auto& b : context.gui->navigation_menu_buttons)
        {
            std::visit(
                [&](auto&& v)
                {
                    const auto& pos = v->pos;
                    if (iu::check_collision(x, y, pos.x, pos.y, pos.w, pos.h))
                    {
                        // calling a lambda, possibly losing a few instructions
                        if (event_type == SDL_MOUSEBUTTONDOWN)
                        {
                            v->clicked(context);
                            return;
                        }

                        context.gui->focused_button = v->button_id;
                        hit = true;
                    }
                },
                b);
        }

        if (!hit) context.gui->focused_button.reset();
    }
}

void rendering::render_tech_view::key_handler(client_context& context, Uint16 k)
{
    if (k == SDLK_ESCAPE)
    {
        context.gui->current_sector.reset();
        context.view = _up();
    }
}

void rendering::render_tech_node(const client_context& context, std::shared_ptr<game_state> gs, tech_pos pos, Technology::TechnologyType tech_type)
{
    auto& r = context.r;
    auto& gr = context.gr;
    auto& gui = context.gui;

    SDL_Color prim = tcol::c_dimmed_main;
    SDL_Color sec = tcol::c_dimmed_secondary;

    if (gs->player.get()->known_technologies.count(tech_type) > 0)
    {
        prim = tcol::c_done_main;
        sec = tcol::c_done_secondary;
    }
    else if (gs->player.get()->available_technologies.count(tech_type) > 0)
    {
        prim = tcol::c_main;
        sec = tcol::c_secondary;
    }

    if (context.gui.get()->hovered_tech == tech_type)
        std::swap(prim, sec);

    constexpr int offset = 4;
    // paint frame

    SDL_SetRenderDrawColor(r.get(), prim.r, prim.g, prim.b, prim.a);
    int dx = size_settings::tech_node_size::width;
    int dy = size_settings::tech_node_size::height;
    int mx = size_settings::tech_node_size::x_offset;
    int my = size_settings::tech_node_size::y_offset;
    SDL_Rect dest{pos.x * dx + mx, pos.y * dy + my, dx - 2 * mx, dy - 2 * my};
    SDL_RenderFillRect(r.get(), &dest);

    SDL_SetRenderDrawColor(r.get(), sec.r, sec.g, sec.b, sec.a);
    dest.x = dest.x + offset;
    dest.y = dest.y + offset;
    dest.w = dest.w - 2 * offset;
    dest.h = dest.h - 2 * offset;
    SDL_RenderFillRect(r.get(), &dest);

    std::string text = Technologies.find(tech_type)->second.name;

    const auto button_offset_x = dest.w;
    sdl_utilities::render_text(r.get(), gr->infobox_font, text, dest.x + dest.w / 2, dest.y + dest.h / 2, dest.w * 0.84, prim);

}