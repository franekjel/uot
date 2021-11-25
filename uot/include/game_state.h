#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <array>
#include <atomic>
#include <optional>
#include <vector>
#include "sdl_utilities.h"
#include "singleton.h"
#include "uncopiable.h"
#include "../../libuot/galaxy.h"

struct game_gui;

struct position
{
    float x;
    float y;
};

enum class game_view_t
{
    menu_view = 0,
    universe_view = 1,
    galaxy_view = 2,
    planet_view = 3,
    ship_view = 4
};

struct game_state_t : public uncopiable
{
   protected:
    game_state_t() { }

    std::shared_ptr<SDL_Texture> bk_texture;
    std::shared_ptr<SDL_Window> window;
    std::shared_ptr<SDL_Renderer> renderer;
    std::atomic<game_view_t> game_view;


    friend singleton<game_state_t>;

    friend game_gui;


   public:
    void draw();
    void reset_galaxy();
    void set_view(game_view_t gv);
    void set_bk_texture(std::shared_ptr<SDL_Texture> bk);
    void set_renderer(std::shared_ptr<SDL_Renderer> r);
    std::shared_ptr<SDL_Renderer>& get_renderer();
    void set_window(std::shared_ptr<SDL_Window> w);
    std::shared_ptr<SDL_Window>& get_window();
    void handleMouse(Uint32 event_type, SDL_MouseButtonEvent m, int x, int y);
    void set_gui();

    int planet_frame = 0;

    std::vector<position> clicked_positions;
    std::unique_ptr<game_gui> gui;;
    std::optional<Galaxy> galaxy;
};



#endif
