#ifndef SDL_UTILITIES_H
#define SDL_UTILITIES_H

#ifdef __linux__
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#elif _WIN32
#include "SDL.h"
#include "SDL_image.h"
#endif

#include <memory>
#include <string>
#include "size_settings.h"
#include "stdexcept"

namespace sdl_utilities
{
std::shared_ptr<SDL_Texture> load_texture_from_file(const std::string& path, const std::shared_ptr<SDL_Renderer>& r);

void paint_background(SDL_Renderer* r, const SDL_Color& c);
void paint_frame(SDL_Renderer* r, const SDL_Color& f, const SDL_Color& b);

std::shared_ptr<SDL_Window> sdl_create_window(const std::string title, int x, int y, int w, int h, Uint32 flags);
std::shared_ptr<SDL_Renderer> sdl_create_renderer(const std::shared_ptr<SDL_Window>& w, int index, Uint32 flags);

template <typename T, int X = T::x_offset, int Y = T::y_offset, int W = T::width, int H = T::height>
void set_render_viewport(SDL_Renderer* r)
{
    SDL_Rect buttonViewport;
    buttonViewport.x = X;
    buttonViewport.y = Y;
    buttonViewport.w = W;
    buttonViewport.h = H;
    SDL_RenderSetViewport(r, &buttonViewport);
}

template <typename T, int X = T::x_offset, int Y = T::y_offset, int W = T::width, int H = T::height>
bool check_view_area_collision(const int x, const int y)
{
    // Mouse is left of the area
    if (x < X || x > X + W || y < Y || y > Y + H)
    {
        return false;
    }

    return true;
}

inline bool check_sector_collision(const int x, const int y, const int box_x, const int box_y, const int w, const int h)
{
    // Mouse is left of the area
    if (x < box_x || x > box_x + w || y < box_y || y > box_y + h)
    {
        return false;
    }

    return true;
}
}  // namespace sdl_utilities

#endif
