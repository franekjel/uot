#include "gui/buttons.h"
#include "client_context.h"
#include "fleet_utils.h"
#include "game_resources.h"
#include "msg_queue.h"
#include "sdl_utilities.h"

namespace rendering
{
void draw_button(client_context& context, const std::string& text, const button_position& pos, button_color scheme,
                 const bool focused)
{
    auto& r = context.r;
    auto& gr = context.gr;
    auto& gui = context.gui;

    int offset = gui->button_border_width.has_value() ? gui->button_border_width.value() : 4;
    // paint frame

    if (gui->button_color_override.has_value())
        scheme = gui->button_color_override.value();

    const SDL_Color& f = focused ? scheme.f_fg : scheme.fg;
    const SDL_Color& b = focused ? scheme.f_bg : scheme.bg;

    SDL_SetRenderDrawColor(r.get(), f.r, f.g, f.b, f.a);
    SDL_Rect dest{pos.x, pos.y, pos.w, pos.h};
    SDL_RenderFillRect(r.get(), &dest);

    SDL_SetRenderDrawColor(r.get(), b.r, b.g, b.b, b.a);
    dest.x = pos.x + offset;
    dest.y = pos.y + offset;
    dest.w = pos.w - 2 * offset;
    dest.h = pos.h - 2 * offset;
    SDL_RenderFillRect(r.get(), &dest);

    const auto button_offset_x = dest.w;
    sdl_utilities::render_text_center(r.get(), gui->button_font.has_value() ? gui->button_font.value() : gr->main_font, text,
                               dest.x + dest.w / 2, dest.y + dest.h / 2, dest.w * 0.8, f);
}
}  // namespace rendering

void start_button::_clicked(client_context& context)
{
    if (Mix_PlayChannel(-1, context.gr->open_ship.get(), 0) == -1)
    {
        throw std::runtime_error("couldnt plat the click sound");
    }
    context.view = std::make_shared<rendering::render_universe_view>();
    context.gui->popup_buttons.erase(context.gui->popup_buttons.begin(), context.gui->popup_buttons.begin() + 1);
    context.gui->popup_buttons.insert(context.gui->popup_buttons.begin(), std::make_unique<back_button>());
}

void back_button::_clicked(client_context& context)
{
    if (Mix_PlayChannel(-1, context.gr->open_ship.get(), 0) == -1)
    {
        throw std::runtime_error("couldnt plat the click sound");
    }
    context.view = std::make_shared<rendering::render_universe_view>();
}

void exit_button::_clicked(client_context& context)
{
    if (Mix_PlayChannel(-1, context.gr->click.get(), 0) == -1)
    {
        throw std::runtime_error("couldnt plat the click sound");
    }
    SDL_Event sdlevent;
    sdlevent.type = SDL_QUIT;
    SDL_PushEvent(&sdlevent);
}

void generic_button::_clicked(client_context& context)
{
    if (Mix_PlayChannel(-1, context.gr->click.get(), 0) == -1)
    {
        throw std::runtime_error("couldnt plat the click sound");
    }
    handler(context);
}

void technology_button::_clicked(client_context& context)
{
    if (Mix_PlayChannel(-1, context.gr->click.get(), 0) == -1)
    {
        throw std::runtime_error("couldnt plat the click sound");
    }
#ifdef UOT_DEBUG
    printf("Technology button clicked\n");
#endif
    context.view = std::make_shared<rendering::render_tech_view>();
}

void designer_button::_clicked(client_context& context)
{
    if (Mix_PlayChannel(-1, context.gr->click.get(), 0) == -1)
    {
        throw std::runtime_error("couldnt plat the click sound");
    }
#ifdef UOT_DEBUG
    printf("Designer button clicked\n");
#endif
    auto v = std::make_shared<rendering::render_designer_view>();
    v->init(context);
    context.view = std::move(v);
}

void research_button::_clicked(client_context& context)
{
    if (!context.gui->current_tech.has_value())
        return;
    context.getActionQueue().value->request_research(context.gui->current_tech.value());
    context.gui->current_tech.reset();
}

void abort_research_button::_clicked(client_context& context)
{
    context.getActionQueue().value->request_research(Technology::TechnologyType::None);
    context.gui->current_tech.reset();
}

void jump_button::_clicked(client_context& context)
{
    if (context.gui->current_fleet.has_value())
        context.getActionQueue().value->request_fleet_jump(context.gui->current_fleet.value()->id);
    context.gui->current_fleet.reset();
}

void build_base_button::_clicked(client_context& context)
{
    context.getActionQueue().value->request_fleet_build(context.gui->current_fleet.value()->id);
}

void colonize_button::_clicked(client_context& context)
{
    context.getActionQueue().value->request_fleet_colonize(context.gui->current_fleet.value()->id);
}

void civil_on_button::_clicked(client_context& context)
{
    context.getActionQueue().value->request_fleet_civil_on(context.gui->current_fleet.value()->id);
}

void civil_off_button::_clicked(client_context& context)
{
    context.getActionQueue().value->request_fleet_civil_off(context.gui->current_fleet.value()->id);
}

void soldier_on_button::_clicked(client_context& context)
{
    context.getActionQueue().value->request_fleet_soldiers_on(context.gui->current_fleet.value()->id);
}

void soldier_off_button::_clicked(client_context& context)
{
    context.getActionQueue().value->request_fleet_soldiers_off(context.gui->current_fleet.value()->id);
}

void invade_button::_clicked(client_context& context)
{
    context.getActionQueue().value->request_fleet_invade(context.gui->current_fleet.value()->id);
}

void fleet_cancel_button::_clicked(client_context& context)
{
    context.getActionQueue().value->request_fleet_cancel(context.gui->current_fleet.value()->id);
}

void jump_button::update_active(client_context& context) { active = has_jump_neighbor(context); }

void build_base_button::update_active(client_context& context) { active = fleet_can_build(context); }
void colonize_button::update_active(client_context& context) { active = fleet_can_colonize(context); }
void civil_on_button::update_active(client_context& context) { active = fleet_can_civils_on(context); }
void civil_off_button::update_active(client_context& context) { active = fleet_can_civils_off(context); }
void soldier_on_button::update_active(client_context& context) { active = fleet_can_soldiers_on(context); }
void soldier_off_button::update_active(client_context& context) { active = fleet_can_soldiers_off(context); }
void invade_button::update_active(client_context& context) { active = fleet_can_invade(context); }
void fleet_cancel_button::update_active(client_context& context) { active = true; }

void update_all_fleet_buttons(std::vector<fleet_button>& buttons, client_context& context)
{
    for (auto& b : buttons)
    {
        std::visit([&](auto&& v) { v->update_active(context); }, b);
    }
}

void set_fleet_button_color(fleet_button& b, client_context& context)
{
    SDL_Color gray = {0x78, 0x78, 0x78, 0xFF};
    SDL_Color white = {0xFF, 0xFF, 0xFF, 0xFF};
    std::visit(
        [&](auto&& v)
        {
            if (!v->is_active())
                context.gui->button_color_override = {white, gray, white, gray};
            else
                context.gui->button_color_override.reset();
        },
        b);
}
