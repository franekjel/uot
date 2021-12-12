#ifndef SDL_UTILITIES_H
#define SDL_UTILITIES_H

#ifdef __linux__
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#elif _WIN32
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#endif

#include <memory>
#include <string>
#include "size_settings.h"
#include "stdexcept"

namespace sdl_utilities
{
void render_text(SDL_Renderer* r, std::shared_ptr<TTF_Font> font, const std::string& t, int x, int y, int w,
                 SDL_Color c);

std::shared_ptr<SDL_Texture> load_texture_from_file(const std::string& path, const std::shared_ptr<SDL_Renderer>& r);

void paint_background(SDL_Renderer* r, const SDL_Color& c);

void paint_frame(SDL_Renderer* r, const SDL_Color& f, const SDL_Color& b);

void paint_frame_textured(SDL_Renderer* r, const SDL_Color& f, std::shared_ptr<SDL_Texture> t);

std::shared_ptr<TTF_Font> load_font(const std::string& font_name, int size);

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
}  // namespace sdl_utilities

#endif