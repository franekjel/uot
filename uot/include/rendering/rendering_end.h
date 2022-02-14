#ifndef UOT_RENDERING_END_H
#define UOT_RENDERING_END_H
#include "rendering_common.h"
#include "rendering_views.h"

namespace rendering
{
struct render_end_view : render_view<render_end_view>
{
    view_t _up();
    view_t _down(client_context& context);
    void _draw(client_context& context);
    void _mouse_handler(client_context& context, Uint32 event_type, SDL_MouseButtonEvent m, int x, int y);
    void key_handler(client_context& context, Uint16 k);
    void _wheel_handler(client_context& context, int x, int y, int xmov, int ymov);
};

}  // namespace rendering

#endif  // UOT_RENDERING_END_H
