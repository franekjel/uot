#ifndef RENDERING_VIEWS_H
#define RENDERING_VIEWS_H
#include "assets.h"
#include "game_gui.h"
#include "rendering.h"
#include "sdl_utilities.h"

struct client_context;

namespace rendering
{
enum class MouseButton
{
    MOUSE_LEFT,
    MOUSE_RIGHT
};

template <typename T>
struct render_view
{
    friend T;
    void draw(client_context& context) { static_cast<T*>(this)->_draw(context); }
    view_t up() { return static_cast<T*>(this)->_up(); };
    view_t down() { return static_cast<T*>(this)->_down(); };
    void mouse_handler(client_context& context, Uint32 event_type, SDL_MouseButtonEvent m, int x, int y)
    {
        static_cast<T*>(this)->_mouse_handler(context, event_type, m, x, y);
    }
    void key_handler(client_context& context, Uint16 k) { static_cast<T*>(this)->_key_handler(); }

   private:
    render_view<T>() = default;
    render_view<T>(const render_view<T>&) = default;
};

}  // namespace rendering
#endif  // RENDERING_VIEWS_H
