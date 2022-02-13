#include "game_gui.h"
#include "client_context.h"
#include "player.h"

int game_gui::GetTextureIndex(std::shared_ptr<SectorObject> p)
{
    auto pl = std::dynamic_pointer_cast<Planet>(p);
    auto io = std::dynamic_pointer_cast<InhabitableObject>(p);
    auto st = std::dynamic_pointer_cast<Star>(p);

    if (pl)
    {
        const auto id = pl->id;
        if ((int)(pl->climate) == 0)  // temperate
        {
            return planet_types::PLANET_NORMAL_1 + (id % 3);
        }

        if ((int)(pl->climate) == 1)  // cold
        {
            return planet_types::PLANET_COLD_1 + (id % 3);
        }

        if ((int)(pl->climate) == 2)  // hot
        {
            return planet_types::PLANET_HOT_1 + (id % 3);
        }
    }

    if (io)
    {
        const auto id = io->id;
        if ((int)(io->object_type) == 0)  // InhabitablePlanet
        {
            return planet_types::NO_ATM_1 + (id % 3);
        }

        if ((int)(io->object_type) == 1)  // GasGiant
        {
            return planet_types::GAS_GIANT_1 + (id % 3);
        }

        if ((int)(io->object_type) == 2)  // Asteroid
        {
            return planet_types::ASTEROID_1 + (id % 3);
        }

        if ((int)(io->object_type) == 3)  // DarkMatterCloud
        {
            return planet_types::DARK_MATTER_1 + (id % 3);
        }
    }

    if (st)
    {
        const auto id = st->id;
        if ((int)(st->star_type) == 0)  // Sunny
        {
            return planet_types::STAR_SUNNY_1 + (id % 3);
        }

        if ((int)(st->star_type) == 1)  // BlueGiant
        {
            return planet_types::STAR_BLUE_GIANT_1 + (id % 3);
        }

        if ((int)(st->star_type) == 2)  // RedGiant
        {
            return planet_types::STAR_RED_GIANT_1 + (id % 3);
        }

        if ((int)(st->star_type) == 3)  // WhiteDwarf
        {
            return planet_types::STAR_WHITE_DWARF_1 + (id % 3);
        }

        if ((int)(st->star_type) == 4)  // BrownDwarf
        {
            return planet_types::STAR_BROWN_DWARF_1 + (id % 3);
        }

        if ((int)(st->star_type) == 5)  // BlackHole
        {
            return planet_types::STAR_BLACK_HOLE_1 + (id % 3);
        }
    }
    return UNDISCOVERED;
}

static void update_cur_sector_info(client_context& context)
{
    if (!context.gui->current_sector.has_value())
    {
        context.gui->cur_sector_info.reset();
        return;
    }

    auto& s = context.gui->current_sector.value();
    sector_client_info info = {0, 0, static_cast<unsigned int>(s->present_fleets.size())};
    for (auto& o : s->objects)
    {
        Planet* p = dynamic_cast<Planet*>(o.second.get());
        if (p)
        {
            info.planet_cnt++;
            continue;
        }

        InhabitableObject* inh = dynamic_cast<InhabitableObject*>(o.second.get());
        if (inh)
            info.deposit_cnt += inh->resource_deposit.size();
    }

    context.gui->cur_sector_info = info;
}

void game_gui::do_gui_per_turn_update(client_context& context)
{
    update_all_fleet_buttons(context.gui->selected_fleet_buttons, context);
    update_cur_sector_info(context);
    player_id_cache = context.getGameState().value->player->id;
}
