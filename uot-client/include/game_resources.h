#ifndef GAME_RESOURCES_H
#define GAME_RESOURCES_H

#include "singleton.h"
#include "uncopiable.h"
#include "sdl_utilities.h"
#include <atomic>
#include <array>
#include <vector>

struct game_resources_t : public uncopiable
{
    protected:

        game_resources_t() {}

        friend singleton<game_resources_t>;

    public:
        std::vector<std::shared_ptr<SDL_Texture>> buttonTextures;
        std::vector<std::shared_ptr<SDL_Texture>> menuItemTextures;
        std::shared_ptr<SDL_Texture> bkTexture;

};

#endif