#ifndef RENDERING_PLANET_H
#define RENDERING_PLANET_H
#include "rendering_common.h"
#include "rendering_views.h"

namespace rendering
{
struct render_planet_view : render_view<render_planet_view>
{
    view_t _up();
    view_t _down();
    void _mouse_handler(client_context& context, Uint32 event_type, SDL_MouseButtonEvent m, int x, int y);
    void key_handler(client_context& context, Uint16 k);
    void _draw(client_context& context);
};

}  // namespace rendering
#endif  // PLANET_RENDERING_H
