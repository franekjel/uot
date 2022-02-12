#include "gui/buttons.h"
#include "client_context.h"
#include "game_resources.h"
#include "msg_queue.h"
#include "sdl_utilities.h"

namespace rendering
{
void draw_button(client_context& context, const std::string& text, const button_position& pos,
                 const button_color scheme, const bool focused)
{
    auto& r = context.r;
    auto& gr = context.gr;
    auto& gui = context.gui;

    constexpr int offset = 4;
    // paint frame

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
    sdl_utilities::render_text(r.get(), gr->main_font, text, dest.x + dest.w / 2, dest.y + dest.h / 2, dest.w * 0.8, f);
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
