#include "rendering_common.h"
#include "client_context.h"

void rendering::render_background(const client_context& context)
{
    auto& r = context.r;
    auto& gr = context.gr;

    SDL_RenderSetViewport(r.get(), nullptr);
    SDL_RenderCopyEx(r.get(), gr.bk_texture.get(), nullptr, nullptr, 0, nullptr, SDL_FLIP_NONE);
}

void rendering::render_button_sprite(ui_button& button, const client_context& context)
{
    auto& r = context.r;
    auto& gr = context.gr;
    auto& gs = context.gs;
    auto& gui = context.gui;

    const auto texture = gr.buttonTextures[button.type];
    SDL_Rect s{0,
               (gui->focused_button.has_value() && gui->focused_button.value() == button.button_id)
                   ? buttons_meta::button_texture_height
                   : 0,
               buttons_meta::button_texture_width, buttons_meta::button_texture_height};
    SDL_Rect d{button.x, button.y, button.w, button.h};
    SDL_RenderCopyEx(r.get(), texture.get(), &s, &d, 0, nullptr, SDL_FLIP_NONE);
}

void rendering::render_planet_helper(const client_context& context, const float size_multiplier, const int x_off,
                                     const int y_off, const texture_t& tex)
{
    auto& r = context.r;
    auto& gs = context.gs;
    auto& view = context.view;
    auto& gui = context.gui;

    SDL_Rect s{(gui->planet_frame / planets_meta::frame_duration) * tex.w, 0, tex.w, tex.h};

    SDL_Rect d{static_cast<int>(x_off - size_multiplier * 0.5f * tex.w),
               static_cast<int>(y_off - size_multiplier * 0.5f * tex.h), static_cast<int>(tex.w * size_multiplier),
               static_cast<int>(tex.h * size_multiplier)};

    SDL_RenderCopyEx(r.get(), tex.t.get(), &s, &d, 0, nullptr, SDL_FLIP_NONE);
}

void rendering::render_resource_bar(const client_context& context)
{
    constexpr std::array<int, 8> positions{0,
                                           0,
                                           resources_meta::single_size,
                                           0,
                                           0,
                                           resources_meta::single_size,
                                           resources_meta::single_size,
                                           resources_meta::single_size};
    constexpr std::array<int, 4> amounts{45, 87, 563, 2};
    constexpr int num_resources = 4;
    int x_off = 20;
    constexpr int y_off = (size_settings::resource_area::height - fonts::resource_font_size) / 2;
    for (int i = 0; i < num_resources; ++i)
    {
        SDL_Rect s{positions[i], positions[i + 1], resources_meta::single_size, resources_meta::single_size};
        SDL_Rect d{x_off, y_off, fonts::resource_font_size, fonts::resource_font_size};
        SDL_RenderCopyEx(context.r.get(), context.gr.resource_texture.get(), &s, &d, 0, nullptr, SDL_FLIP_NONE);

        x_off += fonts::resource_font_size;
        sdl_utilities::render_text(context.r.get(), context.gr.resource_font, ": " + std::to_string(amounts[i]), x_off,
                                   y_off, 150, {0xFF, 0xFF, 0xFF, 0xFF});
        x_off += 180;
    }
}
