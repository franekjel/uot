#ifndef RENDERING_MENU_H
#define RENDERING_MENU_H
#include "client_context.h"
#include "rendering_common.h"
#include "rendering_views.h"

namespace rendering
{

struct render_menu_view : render_view<render_menu_view>
{
    view_t _up();
    view_t _down();
    void _draw(const client_context& context);
    void _mouse_handler(client_context& context, Uint32 event_type, SDL_MouseButtonEvent m, int x, int y);
    void key_handler(client_context& context, Uint16 k);
};

void render_current_popup_menu(const client_context& context);

}  // namespace rendering
#endif  // MENU_RENDERING_H
