#ifndef BUTTONS_H
#define BUTTONS_H
#include <functional>
#include <memory>
#include <string>
#include <variant>
#include "../../constants/assets.h"
#include "../../constants/size_settings.h"
#include "../../utilities/sdl_include.h"

struct client_context;

struct button_color
{
    SDL_Color fg;
    SDL_Color bg;
    SDL_Color f_fg;
    SDL_Color f_bg;
};

struct button_position
{
    int x{0};
    int y{0};
    int w{0};
    int h{0};
};

namespace bcol
{
inline constexpr SDL_Color c_black{0x00, 0x00, 0x00, 0xFF};
inline constexpr SDL_Color c_white{0xFF, 0xFF, 0xFF, 0xFF};

inline constexpr button_color basic{c_white, c_black, c_black, c_white};

}  // namespace bcol

namespace bpos
{
inline constexpr button_position start_pos{(size_settings::popup_menu_area::width - buttons_meta::button_width) / 2,
                                           planets_meta::frame_height / 2 + 60 + buttons_meta::button_y_offset,
                                           buttons_meta::button_width, buttons_meta::button_height};

inline constexpr button_position exit_pos{(size_settings::popup_menu_area::width - buttons_meta::button_width) / 2,
                                          planets_meta::frame_height / 2 + 60 + 2 * buttons_meta::button_y_offset,
                                          buttons_meta::button_width, buttons_meta::button_height};

inline constexpr button_position tech_pos{40, 40, size_settings::context_area::width - 80,
                                          buttons_meta::button_y_offset};

inline constexpr button_position research_pos{40, size_settings::context_area::height * 3 / 4, size_settings::context_area::width - 80,
                                          buttons_meta::button_y_offset};
}  // namespace bpos

namespace rendering
{
void draw_button(client_context& context, const std::string& text, const button_position& pos,
                 const button_color scheme, const bool focused);
}

template <typename T>
struct button
{
    friend T;
    int button_id;
    std::string text;

    button_position pos;

    button_color color_scheme{bcol::basic};

    void clicked(client_context& context) { static_cast<T*>(this)->_clicked(context); }
    void draw(client_context& context, bool focused)
    {
        rendering::draw_button(context, text, pos, color_scheme, focused);
    }

   private:
    button<T>() = default;
    button<T>(const button<T>& b) = default;
};

struct start_button : button<start_button>
{
    start_button() : button<start_button>{0, std::string("START"), bpos::start_pos, bcol::basic} {}
    void _clicked(client_context& context);
};

struct exit_button : button<exit_button>
{
    exit_button() : button<exit_button>{1, std::string("EXIT"), bpos::exit_pos, bcol::basic} {}
    void _clicked(client_context& context);
};

struct generic_button : button<generic_button>
{
    typedef std::function<void(client_context& context)> handler_t;

    handler_t handler;
    generic_button(handler_t h, std::string t, button_position pos)
        : button<generic_button>{2, t, pos, bcol::basic}, handler(h)
    {
    }
    void _clicked(client_context& context);
};

struct technology_button : button<technology_button>
{
    technology_button() : button<technology_button>{2, std::string("TECH"), bpos::tech_pos, bcol::basic} {}
    void _clicked(client_context& context);
};

struct research_button : button<research_button>
{
    research_button() : button<research_button>{3, std::string("RESEARCH"), bpos::research_pos, bcol::basic} {}
    void _clicked(client_context& context);
};

using popup_button = std::variant<std::unique_ptr<start_button>, std::unique_ptr<exit_button>>;
using navigation_button = std::variant<std::unique_ptr<technology_button>>;

#endif  // BUTTONS_H
