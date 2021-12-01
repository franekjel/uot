#ifndef TEXTURE_H
#define TEXTURE_H
#include "sdl_utilities.h"
#include <memory>

struct texture_t {
    int w, h;
    std::shared_ptr<SDL_Texture> t;
};

#endif