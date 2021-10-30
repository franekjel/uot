#ifndef UTILITIES_H
#define UTILITIES_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "size_settings.h"
#include "stdexcept"
#include <string>
#include <memory>

namespace sdl_utilities {
    std::shared_ptr<SDL_Texture> load_texture_from_file(const std::string& path, const std::shared_ptr<SDL_Renderer>& r);

    void paint_background(SDL_Renderer* r, const SDL_Color& c);

    std::shared_ptr<SDL_Window> sdl_create_window(const std::string title, int x, int y, int w, int h, Uint32 flags);
    std::shared_ptr<SDL_Renderer> sdl_create_renderer(const std::shared_ptr<SDL_Window>& w, int index, Uint32 flags);


    template<typename T, int X = T::x_offset, int Y = T::y_offset, int W = T::width, int H = T::height>
    void set_render_viewport(SDL_Renderer* r) {
        SDL_Rect buttonViewport;
        buttonViewport.x = X;
        buttonViewport.y = Y;
        buttonViewport.w = W;
        buttonViewport.h = H;
        SDL_RenderSetViewport( r, &buttonViewport );
    }
}

#endif
