#ifndef SDL_UTILITIES_H
#define SDL_UTILITIES_H

#include <array>
#include <memory>
#include <string>
#include "SDL_mixer.h"
#include "sdl_include.h"
#include "size_settings.h"
#include "stdexcept"

namespace sdl_utilities
{
void render_text_center(SDL_Renderer* r, std::shared_ptr<TTF_Font>& font, const std::string& t, int x, int y, int w,
                        SDL_Color c);

void render_text_top_left(SDL_Renderer* r, std::shared_ptr<TTF_Font>& font, const std::string& t, int x, int y, int w,
                          SDL_Color c);

void render_text_top_center(SDL_Renderer* r, std::shared_ptr<TTF_Font>& font, const std::string& t, int x, int y, int w,
                            SDL_Color c);

std::shared_ptr<SDL_Texture> load_texture_from_file(const std::string& path, const std::shared_ptr<SDL_Renderer>& r);
std::shared_ptr<SDL_Texture> load_and_paint_texture_from_file(const std::string& path,
                                                              const std::shared_ptr<SDL_Renderer>& r,
                                                              const SDL_Color& c);

std::shared_ptr<SDL_Texture> load_texture_from_svg(const std::string& svg, const std::shared_ptr<SDL_Renderer>& r);

void paint_background(SDL_Renderer* r, const SDL_Color& c);

void paint_frame(SDL_Renderer* r, const SDL_Color& f, const SDL_Color& b);

void paint_frame_textured(SDL_Renderer* r, const SDL_Color& f, std::shared_ptr<SDL_Texture> t);

std::shared_ptr<TTF_Font> load_font(const std::string& font_name, int size);

std::shared_ptr<Mix_Chunk> load_chunk(const std::string& filename);
std::shared_ptr<Mix_Music> load_music(const std::string& filename);

std::shared_ptr<SDL_Window> sdl_create_window(const std::string title, int x, int y, int w, int h, Uint32 flags);
std::shared_ptr<SDL_Renderer> sdl_create_renderer(const std::shared_ptr<SDL_Window>& w, int index, Uint32 flags);

template <typename T, int offset, int X = T::x_offset, int Y = T::y_offset, int W = T::width, int H = T::height>
void set_custom_viewport(SDL_Renderer* r)
{
    SDL_Rect buttonViewport;
    buttonViewport.x = X + offset;
    buttonViewport.y = Y + offset;
    buttonViewport.w = W - 2 * offset;
    buttonViewport.h = H - 2 * offset;
    SDL_RenderSetViewport(r, &buttonViewport);
}

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

void set_viewport(SDL_Renderer* r, int x, int y, int w, int h);

}  // namespace sdl_utilities

namespace colors_meta
{
constexpr int num = 16;
constexpr int num_mod = 15;
constexpr std::array<SDL_Color, 16> user_color{
    SDL_Color{0x00, 0x00, 0x00, 0xFF}, SDL_Color{0xFF, 0x00, 0x00, 0xFF}, SDL_Color{0xFF, 0xFF, 0x00, 0xFF},
    SDL_Color{0xFF, 0xFF, 0xFF, 0xFF}, SDL_Color{0x00, 0xFF, 0xFF, 0xFF}, SDL_Color{0xFF, 0x00, 0xFF, 0xFF},
    SDL_Color{0x00, 0xFF, 0x00, 0xFF}, SDL_Color{0x00, 0x00, 0xFF, 0xFF}, SDL_Color{0x00, 0x00, 0x00, 0xFF},
    SDL_Color{0x00, 0x00, 0x00, 0xFF}, SDL_Color{0x00, 0x00, 0x00, 0xFF}, SDL_Color{0x00, 0x00, 0x00, 0xFF},
    SDL_Color{0x00, 0x00, 0x00, 0xFF}, SDL_Color{0x00, 0x00, 0x00, 0xFF}, SDL_Color{0x00, 0x00, 0x00, 0xFF},
    SDL_Color{0x00, 0x00, 0x00, 0xFF}};
}  // namespace colors_meta

#endif
