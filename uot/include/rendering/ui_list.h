#ifndef UI_LIST_H
#define UI_LIST_H
#include <chrono>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "gui/buttons.h"

struct client_context;

struct ui_list_state
{
    int offset{0};
    std::optional<int> selected_elem;
    std::vector<std::string> elems;
    generic_button action_button;
    int w;
    int w_off;
    int h;
    int h_off;
    Uint32 last_click{0u};

    void handle_click(const int x, const int y);
    void handle_timed_click(client_context& context, const int x, const int y);
    std::optional<int> handle_motion(const int x, const int y);
    ui_list_state(std::vector<std::string>& _e, generic_button ab, int _w, int _w_off, int _h, int _h_off)
        : elems(_e), action_button(ab), w(_w), w_off(_w_off), h(_h), h_off(_h_off)
    {
    }
};

void render_list(client_context& context, std::shared_ptr<ui_list_state> l_st, const bool use_button = true);

#endif
