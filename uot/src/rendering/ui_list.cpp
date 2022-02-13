#include "ui_list.h"
#include "client_context.h"
#include "game_resources.h"
#include "input_utilities.h"
#include "sdl_utilities.h"

namespace iu = input_utilities;

void render_list(client_context& context, std::shared_ptr<ui_list_state> l_st, const bool use_button)
{
    // render l_st elements
    auto& r = context.r;
    auto& gr = context.gr;
    auto& gui = context.gui;

    if (use_button)
    {
        l_st->action_button.draw(context, false);
    }

    SDL_Rect curr_view;
    SDL_RenderGetViewport(r.get(), &curr_view);
    auto old_view{curr_view};
    curr_view.y += l_st->action_button.pos.h / 2;
    curr_view.h = use_button ? l_st->action_button.pos.y - l_st->action_button.pos.h / 2 - 15
                             : curr_view.h - l_st->action_button.pos.h;

    SDL_RenderSetViewport(r.get(), &curr_view);

    int curr = 0;
    for (const auto& e : l_st->elems)
    {
        bool focused = l_st->selected_elem.has_value() && l_st->selected_elem.value() == curr;

        const SDL_Color t = {255, 255, 255, 255};
        const SDL_Color b = focused ? SDL_Color{140, 140, 140, 255} : SDL_Color{200, 200, 200, 255};

        // render rectangle
        SDL_SetRenderDrawColor(r.get(), b.r, b.g, b.b, b.a);
        SDL_Rect dest{l_st->w_off, l_st->h_off + (l_st->h + l_st->h_off) * curr - l_st->offset, l_st->w, l_st->h};
        SDL_RenderFillRect(r.get(), &dest);
        // render text
        sdl_utilities::render_text(r.get(), gr->action_button_font, e, dest.x + dest.w / 2, dest.y + dest.h / 2,
                                   dest.w * 0.7, t);
        // inc position
        ++curr;
    }
    // render action button
}

void ui_list_state::handle_click(const int x, const int y)
{
    int curr = 0;
    for (const auto& e : elems)
    {
        SDL_Rect curr_elem{w_off, action_button.pos.h / 2 + h_off + (h_off + h) * curr - offset, w, h};
        if (iu::check_collision(x, y, curr_elem.x, curr_elem.y, curr_elem.w, curr_elem.h))
        {
            selected_elem = curr;
            return;
        }
        ++curr;
    }
    selected_elem.reset();
}

void ui_list_state::handle_timed_click(client_context& context, const int x, const int y)
{
    int curr = 0;
    for (const auto& e : elems)
    {
        SDL_Rect curr_elem{w_off, action_button.pos.h / 2 + h_off + (h_off + h) * curr - offset, w, h};
        if (iu::check_collision(x, y, curr_elem.x, curr_elem.y, curr_elem.w, curr_elem.h))
        {
            if (selected_elem.has_value() && selected_elem.value() == curr)
            {
                auto curr_time = SDL_GetTicks() - last_click;
                if (curr_time < 500)
                {
                    action_button.clicked(context);
                }
                last_click = SDL_GetTicks();
                return;
            }
            // implicit else
            selected_elem = curr;
            return;
        }
        ++curr;
    }
    selected_elem.reset();
}

std::optional<int> ui_list_state::handle_motion(const int x, const int y)
{
    std::optional<int> ret;
    int curr = 0;
    for (const auto& e : elems)
    {
        SDL_Rect curr_elem{w_off, action_button.pos.h / 2 + h_off + (h_off + h) * curr - offset, w, h};
        if (iu::check_collision(x, y, curr_elem.x, curr_elem.y, curr_elem.w, curr_elem.h))
        {
            ret = curr;
            return ret;
        }
        ++curr;
    }

    return ret;
}
