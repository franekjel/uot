#ifndef TEXTURE_H
#define TEXTURE_H
#include <memory>
#include "sdl_utilities.h"

struct texture_t
{
    int w, h;
    std::shared_ptr<SDL_Texture> t;
};

#endif