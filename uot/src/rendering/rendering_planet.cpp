#include "rendering/rendering_planet.h"
#include "client_context.h"
#include "game_gui.h"
#include "game_resources.h"
#include "game_state.h"
#include "planet.h"
#include "rendering_common.h"
#include "utilities/input_utilities.h"

void rendering::render_planet_view::_draw(client_context& context)
{
    render_background(context);
    // draw the above astronaut buttons
    auto state = context.getGameState();
    auto& gs = state.value;
    auto& gr = context.gr;
    auto& r = context.r;
    auto& gui = context.gui;

    sdl_utilities::set_render_viewport<size_settings::resource_area>(r.get());
    sdl_utilities::paint_background(r.get(), SDL_Color{0x00, 0x00, 0x00, 150});
    render_resource_bar(context);

    // draw the left, main game panel
    sdl_utilities::set_render_viewport<size_settings::planet_play_area>(r.get());
    sdl_utilities::paint_frame_textured(r.get(), SDL_Color{0xFF, 0xFF, 0xFF, 0xFF}, gr->sky_square_texture);
    sdl_utilities::set_custom_viewport<size_settings::planet_play_area, size_settings::frame_size>(r.get());
    rendering::render_planet_helper(context, 10.0f, size_settings::planet_play_area::width / 6,
                                    size_settings::planet_play_area::height / 1.6,
                                    gr->planetTextures[11 + context.gui->current_object.value()->id % 18]);
    sdl_utilities::set_custom_viewport<size_settings::planet_play_area, size_settings::frame_size>(r.get());
    sdl_utilities::paint_background(r.get(), SDL_Color{0x00, 0x00, 0x00, 150});

    // render all 5 building places
    const auto planet = std::dynamic_pointer_cast<Planet>(gui->current_object.value());
    for (auto b = Building::PlanetaryAdministration; b <= Building::Greenhouses;
         b = static_cast<Building::BuildingType>(static_cast<int>(b) + 1))
    {
        // None is 0
        int b_idx = b - 1;
        SDL_Rect s{buildings_meta::sprite_positions[b_idx].x, buildings_meta::sprite_positions[b_idx].y,
                   buildings_meta::sprite_positions[b_idx].w, buildings_meta::sprite_positions[b_idx].h};
        SDL_Rect d{buildings_meta::render_positions[b_idx].x, buildings_meta::render_positions[b_idx].y,
                   buildings_meta::render_positions[b_idx].w, buildings_meta::render_positions[b_idx].h};
        SDL_RenderCopyEx(context.r.get(), context.gr->buildings_blur_sprite.get(), &s, &d, 0, nullptr, SDL_FLIP_NONE);

        if (planet && planet->possible_buildings.count(b) > 0)
        {
            // render BUILD ME
            sdl_utilities::render_text(r.get(), gr->secondary_font, "BUILD ME",
                                       buildings_meta::render_positions[b_idx].x,
                                       buildings_meta::render_positions[b_idx].y,
                                       buildings_meta::render_positions[b_idx].w, SDL_Color{0x00, 0xFF, 0x00, 0xFF});
            continue;
        }

        // render NOT AVAILABLE
        sdl_utilities::render_text(r.get(), gr->secondary_font, "IMPOSSIBLE", buildings_meta::render_positions[b_idx].x,
                                   buildings_meta::render_positions[b_idx].y - fonts::secondary_font_size, 200,
                                   SDL_Color{0xFF, 0x00, 0x00, 0xFF});
    }
    // PlanetaryAdministration,
    // ImprovedMetalsMine,
    // MetalsMine,
    // Farm,
    // Greenhouses

    // =======================================================
    // draw the right, information / GUI panel
    sdl_utilities::set_render_viewport<size_settings::planet_context_area>(r.get());
    sdl_utilities::paint_frame(r.get(), SDL_Color{0xFF, 0xFF, 0xFF, 0xFF}, SDL_Color{0x00, 0x00, 0x00, 0xFF});
    if (gui->current_building.has_value())
    {
        // render full sprite
        int b_idx = gui->current_building.value() - 1;
        SDL_Rect s{buildings_meta::sprite_positions[b_idx].x, buildings_meta::sprite_positions[b_idx].y,
                   buildings_meta::sprite_positions[b_idx].w, buildings_meta::sprite_positions[b_idx].h};
        SDL_Rect d{size_settings::planet_context_area::width / 2 - buildings_meta::sprite_positions[b_idx].w,
                   buildings_meta::sprite_positions[b_idx].h * 2, buildings_meta::sprite_positions[b_idx].w * 2,
                   buildings_meta::sprite_positions[b_idx].h * 2};
        SDL_RenderCopyEx(context.r.get(), context.gr->buildings_sprite.get(), &s, &d, 0, nullptr, SDL_FLIP_NONE);

        sdl_utilities::render_text(r.get(), gr->secondary_font, std::string(buildings_meta::names[b_idx]),
                                   size_settings::planet_context_area::width / 2,
                                   size_settings::planet_context_area::height - 100,
                                   size_settings::planet_context_area::width, SDL_Color{0xFF, 0xFF, 0xFF, 0xFF});
    }
}

rendering::view_t rendering::render_planet_view::_up() { return std::make_shared<render_sector_view>(); }

rendering::view_t rendering::render_planet_view::_down() { return std::make_shared<render_planet_view>(); }

void rendering::render_planet_view::_mouse_handler(client_context& context, Uint32 event_type, SDL_MouseButtonEvent m,
                                                   int x, int y)
{
    namespace iu = input_utilities;
    const auto et = input_utilities::get_planet_event_type(event_type, m, x, y);
    auto& curr_b = context.gui->current_building;
    if (et == iu::uot_event_type::planet_left_click_play)
    {
        using AreaType = size_settings::planet_play_area;
        x = x - AreaType::x_offset;
        y = y - AreaType::y_offset;

        for (auto b = Building::PlanetaryAdministration; b <= Building::Greenhouses;
             b = static_cast<Building::BuildingType>(static_cast<int>(b) + 1))
        {
            int b_ind = b - 1;
            const auto tex_size = planets_meta::texture_size[GAS_GIANT_1];
            if (iu::check_collision(
                    x, y, buildings_meta::render_positions[b_ind].x, buildings_meta::render_positions[b_ind].y,
                    buildings_meta::render_positions[b_ind].w, buildings_meta::render_positions[b_ind].h))
            {
                curr_b = b;
                return;
            }
        }
        context.gui->current_building.reset();
    }
}

void rendering::render_planet_view::key_handler(client_context& context, Uint16 k)
{
    if (k == SDLK_ESCAPE)
    {
        context.gui->current_building.reset();
        context.view = _up();
    }
}
