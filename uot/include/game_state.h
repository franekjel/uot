#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "singleton.h"
#include "uncopiable.h"
#include "sdl_utilities.h"
#include <atomic>
#include <vector>
#include <array>
#include <optional>

struct position {
    float x;
    float y;
};

enum class game_view_t {
    menu_view = 0,
    universe_view = 1,
    galaxy_view = 2,
    planet_view = 3,
    ship_view = 4
};

struct game_state_t : public uncopiable
{
    protected:

        game_state_t() {}

        std::shared_ptr<SDL_Texture> bk_texture;
        std::shared_ptr<SDL_Window> window;
        std::shared_ptr<SDL_Renderer> renderer;

        std::atomic<game_view_t> game_view;

        std::atomic<unsigned int> current_object;
        std::atomic<unsigned int> current_ship;

        friend singleton<game_state_t>;

    public:
        void draw();
        void handleMouse(Uint32 event_type, int x, int y);
        void set_view(game_view_t gv);
        void set_bk_texture(std::shared_ptr<SDL_Texture> bk);
        void set_renderer(std::shared_ptr<SDL_Renderer> r);
        std::shared_ptr<SDL_Renderer>& get_renderer();
        void set_window(std::shared_ptr<SDL_Window> w);
        std::shared_ptr<SDL_Window>& get_window();

        int planet_frame = 0;
        std::optional<int> focused_button;

        std::vector<position> clicked_positions;
};

#endif
