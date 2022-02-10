#include "fleet_utils.h"
#include <input_utilities.h>
#include "ship.h"

namespace iu = input_utilities;

static Point get_fleet_abs_pos(std::shared_ptr<Fleet> fleet)
{
    Point v = fleet->position;

    v.x = size_settings::play_area::width / 2 + (size_settings::play_area::height / 2) * v.x;
    v.y = size_settings::play_area::height / 2 + (size_settings::play_area::height / 2) * v.y;

    return v;
}

std::shared_ptr<Sector> jump_neighbor(client_context& context)
{
    auto gui = context.gui;

    if (gui->current_fleet.has_value())
    {
        Point f_pos = get_fleet_abs_pos(gui->current_fleet.value());
        const auto r = selection_meta::bd_w / 12;

        const auto& curr = gui->current_sector.value();
        for (const auto& neighbor : gui->current_sector.value()->neighbors)
        {
            const auto unit = (neighbor->position - gui->current_sector.value()->position).normalized();
            const auto offset = unit * 0.5 * size_settings::play_area::height;
            Point s_pos = {size_settings::play_area::width / 2 + offset.x, size_settings::play_area::height / 2 + offset.y};

            if (iu::check_collision_circle(static_cast<int>(f_pos.x), static_cast<int>(f_pos.y), static_cast<int>(s_pos.x), static_cast<int>(s_pos.y), static_cast<int>(r)))
            {
                std::cout << "Detected collision with sector " << neighbor->sector_id << "\n";
                return neighbor;
            }
        }
    }
    return std::shared_ptr<Sector>();
}
