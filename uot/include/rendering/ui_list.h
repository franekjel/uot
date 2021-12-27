#ifndef UI_LIST_H
#define UI_LIST_H
#include <optional>
#include <memory>
#include <vector>
#include <string>
#include "gui/buttons.h"

struct client_context;

struct ui_list_state {
    int offset { 0 };
    std::optional<int> selected_elem;
    std::vector<std::string>& elems;
    generic_button action_button;

    void handle_click(const int x, const int y);
    ui_list_state(std::vector<std::string>& _e, generic_button ab) 
        : elems(_e), action_button(ab) {}
};

void render_list(client_context& context, std::shared_ptr<ui_list_state> l_st);

#endif
