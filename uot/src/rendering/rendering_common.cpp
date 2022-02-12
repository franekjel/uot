#include "rendering_common.h"
#include "client_context.h"
#include "game_gui.h"
#include "game_resources.h"
#include "game_state.h"
#include "player.h"

void rendering::render_background(const client_context& context)
{
    auto& r = context.r;
    auto& gr = context.gr;

    SDL_RenderSetViewport(r.get(), nullptr);
    SDL_RenderCopyEx(r.get(), gr->bk_texture.get(), nullptr, nullptr, 0, nullptr, SDL_FLIP_NONE);
}

int rendering::GetTextureIndex(std::shared_ptr<SectorObject> p)
{
    auto pl = std::dynamic_pointer_cast<Planet>(p);
    auto io = std::dynamic_pointer_cast<InhabitableObject>(p);
    auto st = std::dynamic_pointer_cast<Star>(p);
   
    if (pl)
    {
        const auto id = (pl->colony && pl->colony->owner) ? pl->colony->owner->id : 0;
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
        const auto id = (io->base && io->base->owner) ? io->base->owner->id : 0;
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
}

void rendering::render_planet_helper(const client_context& context, const float size_multiplier, const int x_off,
                                     const int y_off, const texture_t& tex)
{
    auto& r = context.r;
    auto& gui = context.gui;

    SDL_Rect s{(gui->planet_frame / planets_meta::frame_duration) * tex.w, 0, tex.w, tex.h};

    SDL_Rect d{static_cast<int>(x_off - size_multiplier * 0.5f * tex.w),
               static_cast<int>(y_off - size_multiplier * 0.5f * tex.h), static_cast<int>(tex.w * size_multiplier),
               static_cast<int>(tex.h * size_multiplier)};

    SDL_RenderCopyEx(r.get(), tex.t.get(), &s, &d, 0, nullptr, SDL_FLIP_NONE);
}

void rendering::render_planet_owner(const client_context& context, const unsigned int owner_id,
                                    const float size_multiplier, const int x_off, const int y_off, const texture_t& tex)
{
    // render ownership color
    auto& r = context.r;
    SDL_SetRenderDrawColor(r.get(), colors_meta::user_color[owner_id & colors_meta::num_mod].r,
                           colors_meta::user_color[owner_id & colors_meta::num_mod].g,
                           colors_meta::user_color[owner_id & colors_meta::num_mod].b,
                           colors_meta::user_color[owner_id & colors_meta::num_mod].a);

    const int _w = size_multiplier * tex.w * 0.3;
    const int _h = size_multiplier * tex.h * 0.1;

    const SDL_Rect d = {static_cast<int>(x_off - 0.5 * _w), static_cast<int>(y_off - 0.5 * _h), _w, _h};
    SDL_RenderFillRect(r.get(), &d);
}

void rendering::render_resource_bar(client_context& context)
{
    auto state = context.getGameState();
    auto& gs = state.value;
    if (!gs->player)
    {
        return;
    }
    constexpr std::array<int, 8> positions{0,
                                           0,
                                           resources_meta::single_size,
                                           0,
                                           0,
                                           resources_meta::single_size,
                                           resources_meta::single_size,
                                           resources_meta::single_size};
    int x_off = 20;
    constexpr int y_off = (size_settings::resource_area::height - fonts::resource_font_size) / 2;
    int i = 0;
    for (auto elem : gs->player->owned_resources)
    {
        SDL_Rect s{positions[(2 * i) & 7], positions[(2 * i + 1) & 7], resources_meta::single_size,
                   resources_meta::single_size};
        SDL_Rect d{x_off, y_off, fonts::resource_font_size, fonts::resource_font_size};
        int row, col;
        switch (elem.first)
        {
            case Resource::Metals:
                row = 9;
                col = 21;
                break;

            case Resource::Antimatter:
                row = 6;
                col = 5;
                break;

            case Resource::RareMetals:
                row = 13;
                col = 21;
                break;

            case Resource::Crystals:
                row = 7;
                col = 20;
                break;

            case Resource::Polymers:
                row = 21;
                col = 2;
                break;

            case Resource::DarkMatter:
                row = 12;
                col = 6;
                break;

            case Resource::AncientNanobots:
                row = 15;
                col = 18;
                break;

            case Resource::AncientRelics:
                row = 17;
                col = 13;
                break;

            case Resource::Spatium:
                row = 21;
                col = 4;
                break;

            case Resource::Food:
                row = 15;
                col = 15;
                break;

            case Resource::Technology:
                row = 16;
                col = 4;
                break;
        }

        SDL_Rect _s{row * resources_meta::single_size, col * resources_meta::single_size, resources_meta::single_size,
                    resources_meta::single_size};
        SDL_Rect _d{x_off, y_off, fonts::resource_font_size, fonts::resource_font_size};
        s = _s;
        d = _d;

        SDL_RenderCopyEx(context.r.get(), context.gr->resource_texture.get(), &s, &d, 0, nullptr, SDL_FLIP_NONE);

        x_off += 20 + fonts::resource_font_size;
        sdl_utilities::render_text(context.r.get(), context.gr->resource_font,
                                   ":" + std::to_string(int(elem.second)) + " |", x_off + fonts::resource_font_size,
                                   y_off + fonts::resource_font_size / 2, 150, {0xFF, 0xFF, 0xFF, 0xFF});
        x_off += 90;
    }
}
