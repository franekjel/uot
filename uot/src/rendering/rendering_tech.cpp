#include "rendering/rendering_tech.h"

#include <deque>

#include "client_context.h"
#include "game_gui.h"
#include "game_resources.h"
#include "game_state.h"
#include "player.h"
#include "rendering/rendering_universe.h"
#include "utilities/input_utilities.h"

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

static int render_subtree_helper(const client_context& context, std::shared_ptr<game_state> gs, tech_pos pos,
                                 Technology::TechnologyType type)
{
    rendering::render_tech_node(context, gs, pos, type);

    SDL_Point line_start = {(pos.x + 1) * size_settings::tech_node_size::width + context.gui->tech_offset.x,
                            pos.y * size_settings::tech_node_size::height + size_settings::tech_node_size::height / 2 +
                                context.gui->tech_offset.y};

    pos.x++;
    for (auto& tt : Technologies.find(type)->second.unlock)
    {
        SDL_Point line_end = {line_start.x, pos.y * size_settings::tech_node_size::height +
                                                size_settings::tech_node_size::height / 2 + context.gui->tech_offset.y};
        SDL_SetRenderDrawColor(context.r.get(), 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderDrawLine(context.r.get(), line_end.x, line_end.y,
                           line_end.x + size_settings::tech_node_size::x_offset, line_end.y);
        SDL_RenderDrawLine(context.r.get(), line_start.x, line_start.y, line_end.x, line_end.y);
        pos.y = render_subtree_helper(context, gs, pos, tt);
    }

    if (Technologies.find(type)->second.unlock.empty())
        pos.y++;
    else
        SDL_RenderDrawLine(context.r.get(), line_start.x - size_settings::tech_node_size::x_offset, line_start.y,
                           line_start.x, line_start.y);

    context.gui->tech_size.x = std::max(context.gui->tech_size.x, pos.x + 1);
    context.gui->tech_size.y = std::max(context.gui->tech_size.y, pos.y + 1);
    return pos.y;
}

static SDL_Rect tech_node_rect(const client_context& context, tech_pos& pos)
{
    int dx = size_settings::tech_node_size::width;
    int dy = size_settings::tech_node_size::height;
    int mx = size_settings::tech_node_size::x_offset;
    int my = size_settings::tech_node_size::y_offset;
    return {pos.x * dx + mx + context.gui->tech_offset.x, pos.y * dy + my + context.gui->tech_offset.y, dx - 2 * mx,
            dy - 2 * my};
}

static std::optional<Technology::TechnologyType> get_subtree_hover(const client_context& context,
                                                                   std::shared_ptr<game_state> gs, tech_pos& pos,
                                                                   Technology::TechnologyType cur_type, SDL_Point mouse)
{
    SDL_Rect dest = tech_node_rect(context, pos);
    if (input_utilities::check_collision(mouse.x, mouse.y, dest.x, dest.y, dest.w, dest.h))
        return cur_type;

    tech_pos loc_pos = pos;
    loc_pos.x++;
    for (auto& tt : Technologies.find(cur_type)->second.unlock)
    {
        auto type = get_subtree_hover(context, gs, loc_pos, tt, mouse);
        if (type.has_value())
            return type;
    }

    pos.y = loc_pos.y;
    if (Technologies.find(cur_type)->second.unlock.empty())
    {
        pos.y++;
    }
    return {};
}

void rendering::render_tech_tree(const client_context& context, std::shared_ptr<game_state> gs)
{
    auto offset = render_subtree_helper(context, gs, {0, 0}, Technology::TechnologyType::Engineering);
    render_subtree_helper(context, gs, {0, offset}, Technology::TechnologyType::Spaceships);
}

void rendering::render_tech_view::_draw(client_context& context)
{
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
        render_tech_node(context, gs, {0, 0}, Technology::TechnologyType::Engineering);
        render_tech_tree(context, gs);
    }
    // =======================================================
    // draw the right, information / GUI panel
    sdl_utilities::set_render_viewport<size_settings::context_area>(r.get());
    sdl_utilities::paint_frame(r.get(), SDL_Color{0xFF, 0xFF, 0xFF, 0xFF}, SDL_Color{0x00, 0x00, 0x00, 0xFF});

    rendering::render_tech_info(context, gs);
}

void rendering::render_tech_info(client_context& context, std::shared_ptr<game_state> gs)
{
    auto& r = context.r;
    auto& gr = context.gr;
    auto& gui = context.gui;

    Technology::TechnologyType tt;
    bool researched = false;

    if (gui->current_tech.has_value())
    {
        tt = gui->current_tech.value();
    }
    else if (gs->player->researched_technology)
    {
        tt = gs->player->researched_technology.technology;
        researched = true;
    }
    else
        return;

    auto tech = Technologies.find(tt)->second;

    sdl_utilities::render_text(r.get(), gr->main_font, tech.name, size_settings::context_area::width / 2,
                               fonts::main_font_size + 30, size_settings::context_area::width - 50,
                               {0xFF, 0xFF, 0xFF, 0xFF});

    if (researched)
    {
        sdl_utilities::render_text(
            r.get(), gr->secondary_font,
            tech.description +
                "\nDays left: " + std::to_string(static_cast<int>(gs->player->researched_technology.progress_left)),
            size_settings::context_area::width / 2, size_settings::context_area::height * 0.42,
            size_settings::context_area::width - 50, {0xFF, 0xFF, 0xFF, 0xFF});

        abort_research_button b;
        b.draw(context, context.gui->focused_button == b.button_id);
    }
    else
    {
        sdl_utilities::render_text(r.get(), gr->secondary_font,
                                   tech.description + "\nCost: " + std::to_string(static_cast<int>(tech.cost)),
                                   size_settings::context_area::width / 2, size_settings::context_area::height * 0.42,
                                   size_settings::context_area::width - 50, {0xFF, 0xFF, 0xFF, 0xFF});

        if (gs->player->researched_technology.technology != tt && gs->player->available_technologies.count(tt) > 0)
        {
            research_button b;
            b.draw(context, context.gui->focused_button == b.button_id);
        }
    }
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
    if (et == iu::uot_event_type::left_click_play || et == iu::uot_event_type::motion_play)
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

        tech_pos pos = {0, 0};
        auto tt = get_subtree_hover(context, gs, pos, Technology::TechnologyType::Engineering, {x, y});
        if (tt.has_value() ||
            (tt = get_subtree_hover(context, gs, pos, Technology::TechnologyType::Spaceships, {x, y})).has_value())
        {
            context.gui->hovered_tech = tt;
            if (et == iu::uot_event_type::left_click_play)
            {
                context.gui->current_tech = tt;
            }
            return;
        }

        context.gui->hovered_tech.reset();
        if (et == iu::uot_event_type::left_click_play)
            context.gui->current_tech.reset();
    }
    else if (et == iu::uot_event_type::left_click_context || et == iu::uot_event_type::motion_context)
    {
        x = x - size_settings::context_area::x_offset;
        y = y - size_settings::context_area::y_offset;

        bool hit = false;
        tech_menu_button b;

        if (context.gui->current_tech.has_value())
            b = std::make_unique<research_button>();
        else
            b = std::make_unique<abort_research_button>();

        std::visit(
            [&](auto&& v)
            {
                const auto& pos = v->pos;
                if (iu::check_collision(x, y, pos.x, pos.y, pos.w, pos.h))
                {
                    if (event_type == SDL_MOUSEBUTTONDOWN)
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

        if (!hit)
            context.gui->focused_button.reset();
    }
}

static void increment_offset(client_context& context, int dx, int dy)
{
    context.gui->tech_offset.y += dy;
    context.gui->tech_offset.y = std::min(0, context.gui->tech_offset.y);
    context.gui->tech_offset.y = std::max(
        (-context.gui->tech_size.y + 1) * size_settings::tech_node_size::height + size_settings::tech_area::height,
        context.gui->tech_offset.y);

    context.gui->tech_offset.x += dx;
    context.gui->tech_offset.x = std::min(0, context.gui->tech_offset.x);
    context.gui->tech_offset.x = std::max(
        (-context.gui->tech_size.x + 1) * size_settings::tech_node_size::width + size_settings::tech_area::width,
        context.gui->tech_offset.x);
}

void rendering::render_tech_view::key_handler(client_context& context, Uint16 k)
{
    switch (k)
    {
        case SDLK_ESCAPE:
            context.gui->current_sector.reset();
            context.view = _up();
            break;
        case SDLK_w:
            increment_offset(context, 0, 25);
            break;
        case SDLK_s:
            increment_offset(context, 0, -25);
            break;
        case SDLK_a:
            increment_offset(context, 25, 0);
            break;
        case SDLK_d:
            increment_offset(context, -25, 0);
            break;
        default:
            break;
    }
}

void rendering::render_tech_node(const client_context& context, std::shared_ptr<game_state> gs, tech_pos pos,
                                 Technology::TechnologyType tech_type)
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
    SDL_Rect dest = tech_node_rect(context, pos);
    SDL_RenderFillRect(r.get(), &dest);

    SDL_SetRenderDrawColor(r.get(), sec.r, sec.g, sec.b, sec.a);
    dest.x = dest.x + offset;
    dest.y = dest.y + offset;
    dest.w = dest.w - 2 * offset;
    dest.h = dest.h - 2 * offset;
    SDL_RenderFillRect(r.get(), &dest);

    std::string text = Technologies.find(tech_type)->second.name;

    const auto button_offset_x = dest.w;
    sdl_utilities::render_text(r.get(), gr->infobox_font, text, dest.x + dest.w / 2, dest.y + dest.h / 2, dest.w * 0.84,
                               prim);
}
