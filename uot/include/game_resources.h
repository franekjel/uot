#ifndef GAME_RESOURCES_H
#define GAME_RESOURCES_H

#include <array>
#include <atomic>
#include <vector>
#include "sdl_utilities.h"
#include "singleton.h"
#include "uncopiable.h"

struct game_resources_t : public uncopiable
{
   protected:
    game_resources_t() {}

    friend singleton<game_resources_t>;

   public:
    std::vector<std::shared_ptr<SDL_Texture>> buttonTextures;
    // access by texture_id
    std::vector<std::shared_ptr<SDL_Texture>> planetTextures;
    std::vector<std::shared_ptr<SDL_Texture>> menuItemTextures;
    std::shared_ptr<SDL_Texture> bkTexture;
};

#endif