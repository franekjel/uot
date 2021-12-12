#include <algorithm>
#include "assets.h"
#include "sdl_utilities.h"

void sdl_texture_deleter(SDL_Texture* t) { SDL_DestroyTexture(t); }

void sdl_font_deleter(TTF_Font* f) { TTF_CloseFont(f); }

void sdl_surface_deleter(SDL_Surface* s) { SDL_FreeSurface(s); }

std::shared_ptr<TTF_Font> sdl_utilities::load_font(const std::string& font_name, int size)
{
    auto ret = std::shared_ptr<TTF_Font>(TTF_OpenFont(font_name.c_str(), size), sdl_font_deleter);
    if (ret == NULL)
    {
        throw std::runtime_error("Unable to load font " + font_name);
    }
    return ret;
}

std::shared_ptr<SDL_Texture> sdl_utilities::load_texture_from_file(const std::string& path,
                                                                   const std::shared_ptr<SDL_Renderer>& r)
{
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == nullptr)
    {
        throw std::runtime_error("Unable to load image %s! SDL_image Error: %s\n" + path + +" " +
                                 std::string(IMG_GetError()));
    }

    SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));
    const auto ret =
        std::shared_ptr<SDL_Texture>(SDL_CreateTextureFromSurface(r.get(), loadedSurface), sdl_texture_deleter);
    if (ret == nullptr)
    {
        throw std::runtime_error("Unable to create texture from %s! SDL Error: %s\n" + path + +" " +
                                 std::string(SDL_GetError()));
    }
    SDL_FreeSurface(loadedSurface);

    return ret;
}

void sdl_utilities::paint_background(SDL_Renderer* r, const SDL_Color& c)
{
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
    SDL_RenderFillRect(r, nullptr);
}

void sdl_utilities::paint_frame_textured(SDL_Renderer* r, const SDL_Color& f, std::shared_ptr<SDL_Texture> t)
{
    constexpr int offset = 8;
    // paint frame
    SDL_SetRenderDrawColor(r, f.r, f.g, f.b, f.a);
    SDL_RenderFillRect(r, nullptr);

    // paint background
    SDL_Rect vp;
    SDL_RenderGetViewport(r, &vp);
    const auto vp_width = vp.w;
    const auto vp_height = vp.h;

    SDL_Rect dest;
    dest.x = vp.x + offset;
    dest.y = vp.y + offset;
    dest.w = vp_width - 2 * offset;
    dest.h = vp_height - 2 * offset;
    SDL_RenderSetViewport(r, &dest);
    SDL_RenderCopyEx(r, t.get(), nullptr, nullptr, 0, nullptr, SDL_FLIP_NONE);
}

void sdl_utilities::paint_frame(SDL_Renderer* r, const SDL_Color& f, const SDL_Color& b)
{
    constexpr int offset = 8;
    // paint frame
    SDL_SetRenderDrawColor(r, f.r, f.g, f.b, f.a);
    SDL_RenderFillRect(r, nullptr);

    // paint background
    SDL_Rect vp;
    SDL_RenderGetViewport(r, &vp);
    const auto vp_width = vp.w;
    const auto vp_height = vp.h;

    SDL_SetRenderDrawColor(r, b.r, b.g, b.b, b.a);
    SDL_Rect dest;
    dest.x = offset;
    dest.y = offset;
    dest.w = vp_width - 2 * offset;
    dest.h = vp_height - 2 * offset;
    SDL_RenderFillRect(r, &dest);
}

void sdl_window_deleter(SDL_Window* w) { SDL_DestroyWindow(w); }

std::shared_ptr<SDL_Window> sdl_utilities::sdl_create_window(const std::string title, int x, int y, int w, int h,
                                                             Uint32 flags)
{
    const auto ret =
        std::shared_ptr<SDL_Window>(SDL_CreateWindow(title.c_str(), x, y, w, h, flags), sdl_window_deleter);

    if (ret == nullptr)
    {
        throw std::runtime_error("Window could not be created! SDL Error: %s\n" + std::string(SDL_GetError()));
    }

    return ret;
}

void sdl_renderer_deleter(SDL_Renderer* r) { SDL_DestroyRenderer(r); }

std::shared_ptr<SDL_Renderer> sdl_utilities::sdl_create_renderer(const std::shared_ptr<SDL_Window>& w, int index,
                                                                 Uint32 flags)
{
    const auto ret = std::shared_ptr<SDL_Renderer>(SDL_CreateRenderer(w.get(), index, flags), sdl_renderer_deleter);
    if (ret == nullptr)
    {
        throw std::runtime_error("Renderer could not be created! SDL Error: %s\n" + std::string(SDL_GetError()));
    }

    SDL_SetRenderDrawBlendMode(ret.get(), SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(ret.get(), 0xFF, 0xFF, 0xFF, 0xFF);

    return ret;
}

void sdl_utilities::render_text(SDL_Renderer* r, std::shared_ptr<TTF_Font> font, const std::string& t, int x, int y,
                                int _w, SDL_Color c)
{
    auto textSurface =
        std::shared_ptr<SDL_Surface>(TTF_RenderText_Blended_Wrapped(font.get(), t.c_str(), c, _w), sdl_surface_deleter);

    if (textSurface == NULL)
    {
        throw std::runtime_error("Unable to generate text surface");
    }

    auto tmp_tex =
        std::shared_ptr<SDL_Texture>(SDL_CreateTextureFromSurface(r, textSurface.get()), sdl_texture_deleter);
    if (tmp_tex == NULL)
    {
        throw std::runtime_error("Unable to generate text texture from surface");
    }

    const auto w = textSurface->w;
    const auto h = textSurface->h;

    SDL_Rect s{0, 0, w, h}, d{x, y, w, h};
    SDL_RenderCopyEx(r, tmp_tex.get(), &s, &d, 0, nullptr, SDL_FLIP_NONE);
}
