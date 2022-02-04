#ifndef RENDERING_TECH
#define RENDERING_TECH
#include "game_state.h"
#include "rendering_common.h"
#include "rendering_views.h"

struct tech_pos
{
    int x;
    int y;
};

namespace tcol
{
inline constexpr SDL_Color c_main{51, 0, 102, 0xFF};
inline constexpr SDL_Color c_secondary{255, 255, 102, 0xFF};
inline constexpr SDL_Color c_dimmed_main{64, 64, 64, 0xFF};
inline constexpr SDL_Color c_dimmed_secondary{200, 200, 180, 0xFF};
inline constexpr SDL_Color c_done_main{0, 102, 102, 0xFF};
inline constexpr SDL_Color c_done_secondary{255, 255, 102, 0xFF};

}  // namespace tcol

namespace rendering
{
struct render_tech_view : render_view<render_tech_view>
{
    view_t _up();
    view_t _down(client_context& context);
    void _draw(client_context& context);
    void _mouse_handler(client_context& context, Uint32 event_type, SDL_MouseButtonEvent m, int x, int y);
    void key_handler(client_context& context, Uint16 k);
    void _wheel_handler(client_context& context, int x, int y, int xmov, int ymov);
};

void render_tech_tree(const client_context& context, std::shared_ptr<game_state> gs);

void render_tech_node(const client_context& context, std::shared_ptr<game_state> gs, tech_pos pos,
                      Technology::TechnologyType tech_type);

void render_tech_info(client_context& context, std::shared_ptr<game_state> gs);

}  // namespace rendering
#endif  // RENDERING_TECH