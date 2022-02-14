#include "rendering_end.h"
#include <sstream>
#include "client_context.h"
#include "player.h"

rendering::view_t rendering::render_end_view::_up() { return rendering::view_t(); }
rendering::view_t rendering::render_end_view::_down(client_context& context) { return rendering::view_t(); }
void rendering::render_end_view::_draw(client_context& context)
{
    render_background(context);
    auto& r = context.r;
    auto& gr = context.gr;
    int width_ex = 600;
    int height_ex = 600;
    int border = 20;
    int width_in = width_ex - 2 * border;
    int height_in = height_ex - 2 * border;
    int offset_x = size_settings::window_area::width / 2;
    int offset_y = size_settings::window_area::height / 2;
    SDL_SetRenderDrawColor(r.get(), 0xF1, 0x4A, 0x16, 0x88);
    SDL_Rect rect = {offset_x - width_ex / 2, offset_y - height_ex / 2, width_ex, height_ex};
    SDL_RenderFillRect(r.get(), &rect);
    rect.w = width_in;
    rect.h = height_in;
    rect.x = offset_x - width_in / 2;
    rect.y = offset_y - height_in / 2;
    SDL_SetRenderDrawColor(r.get(), 0x37, 0x06, 0x65, 0xFF);
    SDL_RenderFillRect(r.get(), &rect);
    bool is_winner = context.getGameState().value->player->is_winner;

    std::string main_str = is_winner ? "You have gained a total control over the galaxy"
                                     : "Your last colony has been taken, your empire lost";

    sdl_utilities::render_text_center(r.get(), gr->main_font, is_winner ? "WIN" : "LOSE", offset_x, offset_y - 200,
                                      width_in - 50, {0xFF, 0xFF, 0xFF, 0xFF});
    sdl_utilities::render_text_center(r.get(), gr->resource_font, main_str, offset_x, offset_y - 80, width_in - 50,
                                      {0xF1, 0x4A, 0x16, 0xFF});

    std::stringstream info;
    auto& player = context.getGameState().value->player;
    info << "Sectors explored: " << player->known_galaxy->sectors.size() << "\n";
    info << "Technologies researched: " << player->known_technologies.size() << "\n";
    if (is_winner)
        info << "Owned colonies: " << player->owned_colonies.size() << "\n";
    info << "Owned bases: " << player->owned_space_bases.size() << "\n";
    info << "Owned fleets: " << player->owned_fleets.size() << "\n";
    sdl_utilities::render_text_center(r.get(), gr->secondary_font, info.str(), offset_x, offset_y + height_in / 4,
                                      width_in - 50, {0xFF, 0xFF, 0xFF, 0xFF});
}
void rendering::render_end_view::_mouse_handler(client_context& context, Uint32 event_type, SDL_MouseButtonEvent m,
                                                int x, int y)
{
}
void rendering::render_end_view::key_handler(client_context& context, Uint16 k)
{
    if (k == SDLK_ESCAPE)
    {
        SDL_Event sdl_event;
        sdl_event.type = SDL_QUIT;
        SDL_PushEvent(&sdl_event);
    }
}
void rendering::render_end_view::_wheel_handler(client_context& context, int x, int y, int xmov, int ymov) {}
