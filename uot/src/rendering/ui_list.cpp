#include "ui_list.h"
#include "client_context.h"
#include "sdl_utilities.h"
#include "game_resources.h"
#include "input_utilities.h"

namespace iu = input_utilities;

void render_list(client_context& context, std::shared_ptr<ui_list_state> l_st) {
    // render l_st elements
    auto& r = context.r;
    auto& gr = context.gr;
    auto& gui = context.gui;

    int curr = 0;
    for(const auto& e : l_st->elems) {
        bool focused = l_st->selected_elem.has_value() &&
            l_st->selected_elem.value() == curr;

        const SDL_Color t = {255, 255, 255, 255};
        const SDL_Color b = focused ? SDL_Color{140, 140, 140, 255} : SDL_Color{200, 200, 200, 255};

        // render rectangle
        SDL_SetRenderDrawColor(r.get(), b.r, b.g, b.b, b.a);
        SDL_Rect dest { 10, 10 + 80 * curr - l_st->offset, 150, 70 };
        SDL_RenderFillRect(r.get(), &dest);
        // render text
        sdl_utilities::render_text(r.get(), gr->main_font, e, dest.x + dest.w / 2, dest.y + dest.h / 2, dest.w * 0.7, t);
        // inc position
        ++curr;
    }
    // render action button
    l_st->action_button.draw(context, false);
}


void ui_list_state::handle_click(const int x, const int y) {
    int curr = 0;
    for(const auto& e : elems) {
        SDL_Rect curr_elem{ 10, 10 + 80 * curr - offset, 150, 70 };
        if(iu::check_collision(x, y,
            curr_elem.x, curr_elem.y, curr_elem.w, curr_elem.h)){
            selected_elem = curr;
            return;
        }
        ++curr;
    }
    selected_elem.reset();
}
