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
        SDL_Rect s{positions[(2 * i) & 7], positions[(2 * i + 1) & 7], resources_meta::single_size, resources_meta::single_size};
        SDL_Rect d{x_off, y_off, fonts::resource_font_size, fonts::resource_font_size};
        if (elem.first == Resource::Metals)
        {
            SDL_Rect _s{9 * resources_meta::single_size, 21 * resources_meta::single_size, resources_meta::single_size,
                        resources_meta::single_size};
            SDL_Rect _d{x_off, y_off, fonts::resource_font_size, fonts::resource_font_size};
            s = _s;
            d = _d;
        }

        if (elem.first == Resource::Antimatter)
        {
            SDL_Rect _s{6 * resources_meta::single_size, 5 * resources_meta::single_size, resources_meta::single_size,
                        resources_meta::single_size};
            SDL_Rect _d{x_off, y_off, fonts::resource_font_size, fonts::resource_font_size};
            s = _s;
            d = _d;
        }

        if (elem.first == Resource::RareMetals)
        {
            SDL_Rect _s{13 * resources_meta::single_size, 21 * resources_meta::single_size, resources_meta::single_size,
                        resources_meta::single_size};
            SDL_Rect _d{x_off, y_off, fonts::resource_font_size, fonts::resource_font_size};
            s = _s;
            d = _d;
        }

        if (elem.first == Resource::Crystals)
        {
            SDL_Rect _s{7 * resources_meta::single_size, 20 * resources_meta::single_size, resources_meta::single_size,
                        resources_meta::single_size};
            SDL_Rect _d{x_off, y_off, fonts::resource_font_size, fonts::resource_font_size};
            s = _s;
            d = _d;
        }

        if (elem.first == Resource::Polymers)
        {
            SDL_Rect _s{21 * resources_meta::single_size, 2 * resources_meta::single_size, resources_meta::single_size,
                        resources_meta::single_size};
            SDL_Rect _d{x_off, y_off, fonts::resource_font_size, fonts::resource_font_size};
            s = _s;
            d = _d;
        }

        if (elem.first == Resource::DarkMatter)
        {
            SDL_Rect _s{12 * resources_meta::single_size, 6 * resources_meta::single_size, resources_meta::single_size,
                        resources_meta::single_size};
            SDL_Rect _d{x_off, y_off, fonts::resource_font_size, fonts::resource_font_size};
            s = _s;
            d = _d;
        }

        if (elem.first == Resource::AncientNanobots)
        {
            SDL_Rect _s{15 * resources_meta::single_size, 18 * resources_meta::single_size, resources_meta::single_size,
                        resources_meta::single_size};
            SDL_Rect _d{x_off, y_off, fonts::resource_font_size, fonts::resource_font_size};
            s = _s;
            d = _d;
        }

        if (elem.first == Resource::AncientRelics)
        {
          
           SDL_Rect _s{17 * resources_meta::single_size, 13 * resources_meta::single_size, resources_meta::single_size,
                       resources_meta::single_size};
           SDL_Rect _d{x_off, y_off, fonts::resource_font_size, fonts::resource_font_size};
           s = _s;
           d = _d;
        }

        if (elem.first == Resource::Spatium)
        {
            SDL_Rect _s{21 * resources_meta::single_size, 4 * resources_meta::single_size, resources_meta::single_size,
                        resources_meta::single_size};
            SDL_Rect _d{x_off, y_off, fonts::resource_font_size, fonts::resource_font_size};
            s = _s;
            d = _d;
        }

        if (elem.first == Resource::Food)
        {
            SDL_Rect _s{15 * resources_meta::single_size, 15 * resources_meta::single_size, resources_meta::single_size,
                        resources_meta::single_size};
            SDL_Rect _d{x_off, y_off, fonts::resource_font_size, fonts::resource_font_size};
            s = _s;
            d = _d;
        }

        if (elem.first == Resource::Technology)
        {
            SDL_Rect _s{16 * resources_meta::single_size, 4 * resources_meta::single_size, resources_meta::single_size,
                        resources_meta::single_size};
            SDL_Rect _d{x_off, y_off, fonts::resource_font_size, fonts::resource_font_size};
            s = _s;
            d = _d;
        }
        

        SDL_RenderCopyEx(context.r.get(), context.gr->resource_texture.get(), &s, &d, 0, nullptr, SDL_FLIP_NONE);

        x_off += 20 + fonts::resource_font_size;
        sdl_utilities::render_text(context.r.get(), context.gr->resource_font,
                                   ":" + std::to_string(int(elem.second)) + " |", x_off + fonts::resource_font_size,
                                   y_off + fonts::resource_font_size / 2, 150, {0xFF, 0xFF, 0xFF, 0xFF});
        x_off += 90;
       // i++;
    }
}
