#ifndef RENDERING_PLANET_H
#define RENDERING_PLANET_H
#include <vector>
#include "buildings.h"
#include "rendering_common.h"
#include "rendering_views.h"
#include "ui_list.h"

namespace rendering
{
struct render_planet_view : render_view<render_planet_view>
{
    enum class planet_sub
    {
        build,
        queue
    };

    view_t _up();
    view_t _down(client_context& context);
    void _mouse_handler(client_context& context, Uint32 event_type, SDL_MouseButtonEvent m, int x, int y);
    void _wheel_handler(client_context& context, int x, int y, int xmov, int ymov);
    void key_handler(client_context& context, Uint16 k);
    void _draw(client_context& context);
    void init(client_context& context);
    void render_planet_info(const client_context& context);
    inline void render_queue_lists(client_context& context);
    inline void render_build_lists(client_context& context);
    inline void refresh_lists(client_context& context, std::shared_ptr<Planet>& pl);

    std::shared_ptr<ui_list_state> queue;
    std::shared_ptr<ui_list_state> built;
    std::shared_ptr<ui_list_state> ships_queue;

    std::shared_ptr<ui_list_state> build;
    std::shared_ptr<ui_list_state> ships_build;

    std::vector<Building::BuildingType> _built;
    std::vector<Building::BuildingType> _queue;
    std::vector<unsigned int> _ships_queue;

    std::vector<Building::BuildingType> _build;
    std::vector<unsigned int> _ships_build;

    struct info_box
    {
        int x, y;
        std::optional<Building::BuildingType> building_type;
        std::optional<unsigned int> design_id;
    };

    std::optional<info_box> box;

    Uint32 info_offset{0u};
    planet_sub current_sub{planet_sub::queue};
};

void render_building_info_box(client_context& context, Building::BuildingType type, int x, int y);
void render_ship_info_box(client_context& context, const unsigned int design_id, int x, int y);

}  // namespace rendering
#endif  // PLANET_RENDERING_H
