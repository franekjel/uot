#include "fleet_utils.h"
#include <input_utilities.h>
#include "player.h"
#include "ship.h"

namespace iu = input_utilities;

struct iPoint
{
    int x;
    int y;
};

static int r = 80;

static iPoint get_screen_pos(Point v)
{
    v.x = size_settings::play_area::width * 0.5f + (size_settings::play_area::height * 0.5f) * v.x;
    v.y = size_settings::play_area::height * 0.5f + (size_settings::play_area::height * 0.5f) * v.y;

    return {static_cast<int>(v.x), static_cast<int>(v.y)};
}

static iPoint get_fleet_abs_pos(std::shared_ptr<Fleet> fleet) { return get_screen_pos(fleet->position); }

bool has_jump_neighbor(client_context& context)
{
    auto gui = context.gui;

    if (gui->current_fleet.has_value())
    {
        iPoint f_pos = get_fleet_abs_pos(gui->current_fleet.value());
        const auto r = selection_meta::bd_w / 12;

        const auto& curr = gui->current_sector.value();
        for (const auto& neighbor : gui->current_sector.value()->neighbors)
        {
            const auto unit = (neighbor->position - gui->current_sector.value()->position).normalized();
            const auto offset = unit * 0.5 * size_settings::play_area::height;
            Point s_pos = {size_settings::play_area::width * 0.5f + offset.x,
                           size_settings::play_area::height * 0.5f + offset.y};

            if (iu::check_collision_circle(f_pos.x, f_pos.y, static_cast<int>(s_pos.x), static_cast<int>(s_pos.y),
                                           static_cast<int>(r)))
            {
                std::cout << "Detected collision with sector " << neighbor->sector_id << "\n";
                return true;
            }
        }
    }
    return false;
}

bool fleet_can_colonize(client_context& context)
{
    if (!context.gui->current_fleet.has_value() || !context.gui->current_sector.has_value())
        return false;
    auto& s = context.gui->current_sector.value();
    auto& f = context.gui->current_fleet.value();
    iPoint f_pos = get_fleet_abs_pos(f);

    for (auto& o : s->objects)
    {
        Planet* p = dynamic_cast<Planet*>(o.second.get());
        if (p == nullptr || p->colony != nullptr)
            continue;
        iPoint p_pos = get_screen_pos(p->position);

        if (input_utilities::check_collision_circle(f_pos.x, f_pos.y, p_pos.x, p_pos.y, r))
            return true;
    }

    return false;
}

bool fleet_can_build(client_context& context)
{
    if (!context.gui->current_fleet.has_value() || !context.gui->current_sector.has_value())
        return false;
    auto& s = context.gui->current_sector.value();
    auto& f = context.gui->current_fleet.value();
    iPoint f_pos = get_fleet_abs_pos(f);

    for (auto& o : s->objects)
    {
        InhabitableObject* p = dynamic_cast<InhabitableObject*>(o.second.get());
        if (p == nullptr || p->resource_deposit.empty() || p->base != nullptr)
            continue;
        iPoint p_pos = get_screen_pos(p->position);

        if (input_utilities::check_collision_circle(f_pos.x, f_pos.y, p_pos.x, p_pos.y, r))
            return true;
    }

    return false;
}

bool fleet_can_civils_on(client_context& context) { return false; }
bool fleet_can_civils_off(client_context& context) { return false; }
bool fleet_can_soldiers_on(client_context& context) { return false; }
bool fleet_can_soldiers_off(client_context& context) { return false; }

bool fleet_can_invade(client_context& context)
{
    if (!context.gui->current_fleet.has_value() || !context.gui->current_sector.has_value())
        return false;
    auto& s = context.gui->current_sector.value();
    auto& f = context.gui->current_fleet.value();
    iPoint f_pos = get_fleet_abs_pos(f);

    for (auto& o : s->objects)
    {
        Planet* p = dynamic_cast<Planet*>(o.second.get());
        if (p == nullptr || p->colony == nullptr || p->colony->owner->id == context.getGameState().value->player->id)
            continue;
        iPoint p_pos = get_screen_pos(p->position);

        if (input_utilities::check_collision_circle(f_pos.x, f_pos.y, p_pos.x, p_pos.y, r))
            return true;
    }

    return false;
}
