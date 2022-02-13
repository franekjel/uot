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
    void render_design_info(client_context& context);
    std::string get_design_cost();
    std::string get_general_info();
    std::string get_design_upkeep();
    std::string get_design_worker_cost();

    inline void refresh_lists(client_context& context);

    std::shared_ptr<ui_list_state> modules_available;
    std::shared_ptr<ui_list_state> modules_chosen;
    std::shared_ptr<ui_list_state> hull;

    std::vector<ModuleType> _available;
    std::vector<ModuleType> _chosen;
    std::vector<ShipHull::Type> _hull;

    std::map<Resource, float> current_costs;
    std::map<Resource, float> current_upkeep;

    int current_worker_weeks{0};

    std::unique_ptr<generic_button> create_design_button;

    Uint32 info_offset{0u};

    struct general_info
    {
        // wolne miejsce
        int space_inside{0};
        int taken_inside{0};
        int space_sides{0};
        int taken_sides{0};
        // hp,
        int hp{0};
        // max energia,
        int max_energy{0};
        // normalny bilans energii,
        int energy_use_normal{0};
        // maksymalne zużycie energii,
        int energy_use_max{0};
        int energy_regen{0};
        // tarcze,
        int shields{0};
        // pojemność ludzi,
        int human_capacity{0};
        // regeneracja hp,
        int hp_regen{0};
        // punkty konstrukcji
        int construction{0};
        int attack{0};
        int engines_energy_consumption{0};
        int speed{0};
        int ship_aggro{0};
    } sh_info;
};

}  // namespace rendering
#endif  // RENDERING_DESIGNER_H
