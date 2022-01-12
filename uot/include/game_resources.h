#ifndef GAME_RESOURCES_H
#define GAME_RESOURCES_H

#include <array>
#include <atomic>
#include <vector>
#include "sdl_utilities.h"
#include "singleton.h"
#include "texture.h"
#include "uncopiable.h"
#include "SDL_mixer.h"


// used for graphics / audio resources
struct game_resources : public uncopiable
{
   protected:
    game_resources() {}

    friend singleton<game_resources>;

   public:
    // access by texture_id
    std::vector<texture_t> planetTextures;
    std::vector<std::shared_ptr<SDL_Texture>> selectionTextures;
    std::vector<std::shared_ptr<SDL_Texture>> menuItemTextures;

    std::shared_ptr<SDL_Texture> bk_texture;
    std::shared_ptr<SDL_Texture> sky_texture;
    std::shared_ptr<SDL_Texture> sky_square_texture;
    std::shared_ptr<SDL_Texture> buildings_sprite;
    std::shared_ptr<SDL_Texture> buildings_blur_sprite;
    std::shared_ptr<SDL_Texture> resource_texture;

    std::shared_ptr<TTF_Font> main_font;
    std::shared_ptr<TTF_Font> secondary_font;
    std::shared_ptr<TTF_Font> resource_font;
    std::shared_ptr<TTF_Font> action_button_font;
    std::shared_ptr<TTF_Font> infobox_font;

    //The music that will be played
    std::shared_ptr<Mix_Music> ambient;

    //The sound effects that will be used
    std::shared_ptr<Mix_Chunk> click;
    std::shared_ptr<Mix_Chunk> open_planet;
    std::shared_ptr<Mix_Chunk> open_ship;
    std::shared_ptr<Mix_Chunk> scanning;

};

#endif
