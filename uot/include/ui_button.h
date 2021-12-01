#ifndef UI_BUTTON_H
#define UI_BUTTON_H

#include <functional>
#include "assets.h"
#include "client_context.h"

struct ui_button
{
    int button_id;
    int x;
    int y;
    int w;
    int h;
    button_types type;

    std::function<void(client_context&)> click_handler;
};

#endif