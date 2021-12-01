#ifndef GAME_RESOURCES_H
#define GAME_RESOURCES_H

#include <array>
#include <atomic>
#include <vector>
#include "sdl_utilities.h"
#include "singleton.h"
#include "texture.h"
#include "uncopiable.h"

// used for graphics / audio resources
struct resource_manager : public uncopiable
{
   protected:
    resource_manager() {}

    friend singleton<resource_manager>;

   public:
    std::vector<std::shared_ptr<SDL_Texture>> buttonTextures;
    // access by texture_id
    std::vector<texture_t> planetTextures;
    std::vector<std::shared_ptr<SDL_Texture>> selectionTextures;
    std::vector<std::shared_ptr<SDL_Texture>> menuItemTextures;

    std::shared_ptr<SDL_Texture> bk_texture;
    std::shared_ptr<SDL_Texture> sky_texture;
    std::shared_ptr<SDL_Texture> resource_texture;

    std::shared_ptr<TTF_Font> main_font;
    std::shared_ptr<TTF_Font> secondary_font;
    std::shared_ptr<TTF_Font> resource_font;
};

#endif