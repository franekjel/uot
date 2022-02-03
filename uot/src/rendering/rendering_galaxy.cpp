#include "rendering_galaxy.h"
#include "client_context.h"
#include "game_gui.h"
#include "game_resources.h"
#include "game_state.h"
#include "input_utilities.h"
#include "player.h"

void rendering::render_sector_view::_wheel_handler(client_context& context, int x, int y, int xmov, int ymov) {}

void rendering::render_sector_view::_draw(client_context& context)
{
    render_background(context);
    // draw the above astronaut buttons
    auto& r = context.r;
    auto& gr = context.gr;
    auto& gui = context.gui;

    sdl_utilities::set_render_viewport<size_settings::resource_area>(r.get());
    sdl_utilities::paint_background(r.get(), SDL_Color{0x00, 0x00, 0x00, 150});
    render_resource_bar(context);

    // =======================================================
    // draw the left, main game panel
    sdl_utilities::set_render_viewport<size_settings::play_area>(r.get());
    sdl_utilities::paint_frame_textured(r.get(), SDL_Color{0xFF, 0xFF, 0xFF, 0xFF}, gr->sky_texture);
    sdl_utilities::set_render_viewport<size_settings::play_area>(r.get());

    SDL_Rect s{0, 0, selection_meta::bd_w, selection_meta::bd_h};
    const auto _w = static_cast<int>(0.8 * size_settings::play_area::height);
    const auto _h = static_cast<int>(0.8 * size_settings::play_area::height);
    SDL_Rect d{size_settings::play_area::width / 2 - _w / 2, size_settings::play_area::height / 2 - _h / 2,
                _w, _h};

    SDL_RenderCopyEx(r.get(), gr->galaxy_boundary.get(), &s, &d,
                     0.0, NULL, SDL_FLIP_NONE);

    if (gui->current_sector.has_value())
    {
        // sector render here
        const auto& curr = gui->current_sector.value();
        render_sector_galaxy_helper(context, curr);
        for(auto& neighbor : gui->current_sector.value()->neighbors) {
            const auto unit = (neighbor->position - gui->current_sector.value()->position).normalized() ;
            const auto offset = unit * 0.45 * size_settings::play_area::height;

            SDL_Rect s{0, 0, selection_meta::bd_w, selection_meta::bd_h};
            const auto _w = selection_meta::bd_w / 12;
            const auto _h = selection_meta::bd_h / 12;
            SDL_Rect d{size_settings::play_area::width / 2 + static_cast<int>(offset.x)
                        - _w / 2, size_settings::play_area::height / 2 + static_cast<int>(offset.y) - _h / 2,
                _w, _h};

            SDL_RenderCopyEx(r.get(), gr->galaxy_boundary.get(), &s, &d,
                     0.0, NULL, SDL_FLIP_NONE);
        }
    }
    render_object_selection(context);
    // =======================================================
    // draw the right, information / GUI panel
    sdl_utilities::set_render_viewport<size_settings::context_area>(r.get());
    sdl_utilities::paint_frame(r.get(), SDL_Color{0xFF, 0xFF, 0xFF, 0xFF}, SDL_Color{0x00, 0x00, 0x00, 0xFF});

    if (gui->current_object.has_value())
    {
        render_selected_object_info(context);
    }
}

void rendering::render_object_selection(const client_context& context)
{
    auto& r = context.r;
    auto& gr = context.gr;
    auto& gui = context.gui;

    if (!gui->current_object.has_value())
    {
        return;
    }

    sdl_utilities::set_custom_viewport<size_settings::play_area, size_settings::frame_size>(r.get());
    sdl_utilities::paint_background(r.get(), SDL_Color{0x00, 0x00, 0x00, 150});
    sdl_utilities::set_render_viewport<size_settings::play_area>(r.get());

    const auto& curr = gui->current_object.value();

    const auto w_filler = (size_settings::play_area::width - 0.8 * size_settings::play_area::height) / 2;
    const int x = w_filler + 0.8 * size_settings::play_area::height * (curr->position.x + 1.0f) / 2;
    const int y = 0.1 * size_settings::play_area::height + 0.8 * size_settings::play_area::height * (curr->position.y + 1.0f) / 2;

    const int tex_id = GAS_GIANT_1 + curr->id % (planets_meta::num_planets - GAS_GIANT_1);
    // render always the smallest possible selection
    const int tex_size = planets_meta::texture_size[GAS_GIANT_1];

    SDL_Rect s{0, 0, selection_meta::texture_width, selection_meta::texture_height};

    SDL_Rect d{static_cast<int>(x - 0.5 * tex_size), static_cast<int>(y - 0.5 * tex_size), tex_size, tex_size};

    SDL_RenderCopyEx(r.get(), gr->selectionTextures[selection_types::SECTOR_SELECTION].get(), &s, &d,
                     SDL_GetTicks() / 100, NULL, SDL_FLIP_NONE);

    render_planet_helper(context, 0.8, x, y, gr->planetTextures[11 + curr->id % 18]);
}

void rendering::render_selected_object_info(const client_context& context)
{
    auto& r = context.r;
    auto& gr = context.gr;
    auto& gui = context.gui;

    auto object_id = GAS_GIANT_1 + gui->current_object.value()->id % (planets_meta::num_planets - GAS_GIANT_1);
    sdl_utilities::render_text(r.get(), gr->main_font, "PLANET NAME", size_settings::context_area::width / 2,
                               fonts::main_font_size / 2 + 30, size_settings::context_area::width - 50,
                               {0xFF, 0xFF, 0xFF, 0xFF});

    // render planet here again
    render_planet_helper(context, 2.0, size_settings::context_area::width / 2,
                         std::min(250, planets_meta::texture_size[object_id] * 2), gr->planetTextures[object_id]);


    auto pl = std::dynamic_pointer_cast<Planet>(gui->current_object.value());
    auto io = std::dynamic_pointer_cast<InhabitableObject>(gui->current_object.value());
    auto st = std::dynamic_pointer_cast<Star>(gui->current_object.value());

    std::string type = pl ? "Planet" : io ? "Inh Object" : "Star";

    // render planet info
    sdl_utilities::render_text(r.get(), gr->secondary_font,
                               " planet index: " + std::to_string(gui->current_object.value()->id) + "\n type " + type +
                                   " \n planet info 2\n planet info 3",
                               size_settings::context_area::width / 2, size_settings::context_area::height * 0.75,
                               size_settings::context_area::width - 50, {0xFF, 0xFF, 0xFF, 0xFF});
}

void rendering::render_sector_galaxy_helper(const client_context& context, const std::shared_ptr<Sector>& sector)
{
    auto& r = context.r;
    auto& gr = context.gr;
    int i = 0;
    const auto w_filler = (size_settings::play_area::width - 0.8 * size_settings::play_area::height) / 2;
    for (const auto& p : sector->objects)
    {
        const auto planet_x = w_filler + 0.8 * size_settings::play_area::height * (1.0f + 1.0f * p->position.x) * 0.5f;
        const auto planet_y = 0.1 * size_settings::play_area::height + 0.8 * size_settings::play_area::height * (1.0f + 1.0f * p->position.y) * 0.5f;
        render_planet_helper(context, 0.8, planet_x, planet_y, gr->planetTextures[11 + p->id % 18]);

        // always the same rec size
        auto pl = std::dynamic_pointer_cast<Planet>(p);
        auto io = std::dynamic_pointer_cast<InhabitableObject>(p);

        if(pl) {
            const auto id = (pl->colony && pl->colony->owner) ? pl->colony->owner->id : 0;
            render_planet_owner(context, id, 0.8, planet_x, planet_y, gr->planetTextures[12]);
        }
        if(io) {
            const auto id = (io->base && io->base->owner) ? io->base->owner->id : 0;
            render_planet_owner(context, id, 0.8, planet_x, planet_y, gr->planetTextures[12]);
        }
    }
}

rendering::view_t rendering::render_sector_view::_up() { return std::make_shared<render_universe_view>(); }

rendering::view_t rendering::render_sector_view::_down(client_context& context)
{
    auto ret = std::make_shared<render_planet_view>();
    ret->init(context);
    return ret;
}

void rendering::render_sector_view::_mouse_handler(client_context& context, Uint32 event_type, SDL_MouseButtonEvent m,
                                                   int x, int y)
{
    auto& gui = context.gui;
    namespace iu = input_utilities;
    const auto et = input_utilities::get_event_type(event_type, m, x, y);
    if (et == iu::uot_event_type::left_click_play)
    {
        using AreaType = size_settings::play_area;
        auto& current_sector = context.gui->current_sector;
        auto& current_object = context.gui->current_object;
        x = x - AreaType::x_offset;
        y = y - AreaType::y_offset;

        const auto& curr = current_sector.value();
        for (const auto& sec_obj : curr->objects)
        {
            const auto& pos = sec_obj->position;
            const auto tex_size = planets_meta::texture_size[GAS_GIANT_1];
            const auto w_filler = (size_settings::play_area::width - 0.8 * size_settings::play_area::height) / 2;
            const auto planet_x = w_filler + 0.8 * size_settings::play_area::height * (1.0f + 1.0f * sec_obj->position.x) * 0.5f;
            const auto planet_y = 0.1 * size_settings::play_area::height + 0.8 * size_settings::play_area::height * (1.0f + 1.0f * sec_obj->position.y) * 0.5f;

            if (iu::check_collision(x, y, planet_x - 0.5 * tex_size,
                                    planet_y - 0.5 * tex_size, tex_size,
                                    tex_size))
            {
                if (current_object.has_value() && current_object.value()->position == sec_obj->position)
                {
                    context.view = down(context);
                    current_object.reset();
                }
                current_object = sec_obj;
                return;
            }
        }
        current_object.reset();


        for(auto& neighbor : gui->current_sector.value()->neighbors) {
            const auto unit = (neighbor->position - gui->current_sector.value()->position).normalized() ;
            const auto offset = unit * 0.45 * size_settings::play_area::height;

            const auto _w = selection_meta::bd_w / 12;
            const auto _h = selection_meta::bd_h / 12;
            const auto _x = size_settings::play_area::width / 2 + static_cast<int>(offset.x)
                        - _w / 2;
            const auto _y = size_settings::play_area::height / 2 + static_cast<int>(offset.y)
                        - _h / 2;

            if(iu::check_collision(x, y, _x, _y, _w, _h)) {
                current_sector = neighbor;
            }
        }
    }
}

void rendering::render_sector_view::key_handler(client_context& context, Uint16 k)
{
    if (k == SDLK_ESCAPE)
    {
        context.gui->current_object.reset();
        context.view = _up();
    }
}
