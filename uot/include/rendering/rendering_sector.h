#ifndef RENDERING_SECTOR_H
#define RENDERING_SECTOR_H
#include "rendering_common.h"
#include "rendering_views.h"

namespace rendering
{
struct render_sector_view : render_view<render_sector_view>
{
    view_t _up();
    view_t _down(client_context& context);
    void _mouse_handler(client_context& context, Uint32 event_type, SDL_MouseButtonEvent m, int x, int y);
    void key_handler(client_context& context, Uint16 k);
    void _draw(client_context& context);
    void _wheel_handler(client_context& context, int x, int y, int xmov, int ymov);
};

void render_sector_sector_helper(const client_context& context, const std::shared_ptr<Sector>& sector);

void render_sector_object();

void render_object_selection(const client_context& context);

void render_selected_object_info(const client_context& context);

void render_fleet(const client_context& context, const std::shared_ptr<Fleet> f);

void render_selected_fleet_info(client_context& context);

void render_selection_graphics(const client_context& context, const Point pos, const int tex_size);

void render_fleet_weapon_ranges(const client_context& context, const Point pos, const std::shared_ptr<Fleet> f);

}  // namespace rendering
#endif  // SECTOR_RENDERING_H
