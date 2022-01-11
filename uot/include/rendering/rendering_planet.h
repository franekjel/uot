#ifndef RENDERING_PLANET_H
#define RENDERING_PLANET_H
#include "rendering_common.h"
#include "rendering_views.h"
#include "ui_list.h"
#include "buildings.h"
#include <vector>

namespace rendering
{
struct render_planet_view : render_view<render_planet_view>
{
    view_t _up();
    view_t _down(client_context& context);
    void _mouse_handler(client_context& context, Uint32 event_type, SDL_MouseButtonEvent m, int x, int y);
    void _wheel_handler(client_context& context, int x, int y, int xmov, int ymov);
    void key_handler(client_context& context, Uint16 k);
    void _draw(client_context& context);
    void init(client_context& context);

    std::shared_ptr<ui_list_state> queue;
    std::shared_ptr<ui_list_state> built;
    std::shared_ptr<ui_list_state> build;

    std::vector<Building::BuildingType> _build;
    std::vector<Building::BuildingType> _built;
    std::vector<Building::BuildingType> _queue;

    struct info_box {
        int x, y;
        Building::BuildingType type;
    };

    std::optional<info_box> box;
};

void render_building_info_box(client_context& context,
                             Building::BuildingType type, int x, int y);

}  // namespace rendering
#endif  // PLANET_RENDERING_H
