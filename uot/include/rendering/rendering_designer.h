#ifndef RENDERING_DESIGNER_H
#define RENDERING_DESIGNER_H
#include <vector>
#include "rendering_common.h"
#include "rendering_views.h"
#include "ui_list.h"

namespace rendering
{
struct render_designer_view : render_view<render_designer_view>
{
    view_t _up();
    view_t _down(client_context& context);

    void _mouse_handler(client_context& context, Uint32 event_type, SDL_MouseButtonEvent m, int x, int y);
    void _wheel_handler(client_context& context, int x, int y, int xmov, int ymov);
    void key_handler(client_context& context, Uint16 k);
    void _draw(client_context& context);
    void init(client_context& context);

    inline void refresh_lists(client_context& context);

    std::shared_ptr<ui_list_state> modules_available;
    std::shared_ptr<ui_list_state> modules_chosen;

    std::vector<ModuleType> _available;
    std::vector<ModuleType> _chosen;

    Uint32 info_offset{0u};
};

}  // namespace rendering
#endif  // RENDERING_DESIGNER_H