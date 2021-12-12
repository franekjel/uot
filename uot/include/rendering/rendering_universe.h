#ifndef RENDERING_UNIVERSE
#define RENDERING_UNIVERSE
#include "rendering_common.h"
#include "rendering_views.h"

namespace rendering
{

struct render_universe_view : render_view<render_universe_view>
{
    view_t _up();
    view_t _down();
    void _draw(const client_context& context);
    void _mouse_handler(client_context& context, Uint32 event_type, SDL_MouseButtonEvent m, int x, int y);
    void key_handler(client_context& context, Uint16 k);
};

void render_sector_universe_helper(const client_context& context, const std::shared_ptr<Sector>& sector);

void render_sector_selection(const client_context& context);

void render_selected_sector_info(const client_context& context);

}  // namespace rendering
#endif  // UNIVERSE_RENDERING_H
