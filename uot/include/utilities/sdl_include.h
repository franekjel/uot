#ifndef SDL_INCLUDE_H
#define SDL_INCLUDE_H

#ifdef __linux__
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#elif _WIN32
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#endif

#endif  // SDL_INCLUDE_H
