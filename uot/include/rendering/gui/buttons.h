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

inline constexpr button_position tech_pos{40, 40, size_settings::context_area::width - 80, buttons_meta::button_height};

inline constexpr button_position designer_pos{40, 40 + buttons_meta::button_height + buttons_meta::button_y_offset,
                                              size_settings::context_area::width - 80, buttons_meta::button_height};

inline constexpr button_position research_pos{40, size_settings::context_area::height * 3 / 4,
                                              size_settings::context_area::width - 80, buttons_meta::button_y_offset};

inline constexpr int fleet_button_width{(size_settings::context_area::width - 80) / 2 + 25};
inline constexpr int fleet_button_height{40};
inline constexpr int fleet_button_margin{10};
inline constexpr int fleet_button_y_offset{size_settings::context_area::height * 3 / 5};

inline constexpr button_position fleet_1_1{10, fleet_button_y_offset, fleet_button_width, fleet_button_height};

inline constexpr button_position fleet_1_2{10 + fleet_button_width + fleet_button_margin, fleet_button_y_offset,
                                           fleet_button_width, fleet_button_height};

inline constexpr button_position fleet_2_1{10, fleet_button_y_offset + fleet_button_height + fleet_button_margin,
                                           fleet_button_width, fleet_button_height};

inline constexpr button_position fleet_2_2{10 + fleet_button_width + fleet_button_margin,
                                           fleet_button_y_offset + fleet_button_height + fleet_button_margin,
                                           fleet_button_width, fleet_button_height};

inline constexpr button_position fleet_3_1{10, fleet_button_y_offset + (fleet_button_height + fleet_button_margin) * 2,
                                           fleet_button_width, fleet_button_height};

inline constexpr button_position fleet_3_2{10 + fleet_button_width + fleet_button_margin,
                                           fleet_button_y_offset + (fleet_button_height + fleet_button_margin) * 2,
                                           fleet_button_width, fleet_button_height};

inline constexpr button_position fleet_4_1{10, fleet_button_y_offset + (fleet_button_height + fleet_button_margin) * 3,
                                           fleet_button_width, fleet_button_height};

inline constexpr button_position fleet_4_2{10 + fleet_button_width + fleet_button_margin,
                                           fleet_button_y_offset + (fleet_button_height + fleet_button_margin) * 3,
                                           fleet_button_width, fleet_button_height};

inline constexpr button_position fleet_5_1{10, fleet_button_y_offset + (fleet_button_height + fleet_button_margin) * 4,
                                           fleet_button_width * 2 + fleet_button_margin, fleet_button_height};

}  // namespace bpos

namespace rendering
{
void draw_button(client_context& context, const std::string& text, const button_position& pos, button_color scheme,
                 const bool focused);
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

struct back_button : button<back_button>
{
    back_button() : button<back_button>{0, std::string("BACK"), bpos::start_pos, bcol::basic} {}
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
    research_button() : button<research_button>{3, std::string("START"), bpos::research_pos, bcol::basic} {}
    void _clicked(client_context& context);
};

struct abort_research_button : button<abort_research_button>
{
    abort_research_button() : button<abort_research_button>{4, std::string("ABORT"), bpos::research_pos, bcol::basic} {}
    void _clicked(client_context& context);
};

struct designer_button : button<designer_button>
{
    designer_button() : button<designer_button>{5, std::string("SHIPS"), bpos::designer_pos, bcol::basic} {}
    void _clicked(client_context& context);
};

using popup_button =
    std::variant<std::unique_ptr<start_button>, std::unique_ptr<exit_button>, std::unique_ptr<back_button>>;
using navigation_button = std::variant<std::unique_ptr<technology_button>, std::unique_ptr<designer_button>>;

struct jump_button : button<jump_button>
{
    jump_button() : button<jump_button>{6, std::string("JUMP"), bpos::fleet_1_1, bcol::basic} {}
    bool active = true;
    bool is_active() { return active; }
    void update_active(client_context& context);
    void _clicked(client_context& context);
};

struct build_base_button : button<build_base_button>
{
    build_base_button() : button<build_base_button>{7, std::string("BUILD"), bpos::fleet_1_2, bcol::basic} {}
    bool active = true;
    bool is_active() { return active; }
    void update_active(client_context& context);
    void _clicked(client_context& context);
};

struct colonize_button : button<colonize_button>
{
    colonize_button() : button<colonize_button>{8, std::string("COLONIZE"), bpos::fleet_2_1, bcol::basic} {}
    bool active = true;
    bool is_active() { return active; }
    void update_active(client_context& context);
    void _clicked(client_context& context);
};

struct civil_on_button : button<civil_on_button>
{
    civil_on_button() : button<civil_on_button>{9, std::string("CIVILIANS ON"), bpos::fleet_2_2, bcol::basic} {}
    bool active = true;
    bool is_active() { return active; }
    void update_active(client_context& context);
    void _clicked(client_context& context);
};

struct civil_off_button : button<civil_off_button>
{
    civil_off_button() : button<civil_off_button>{10, std::string("CIVILIANS OFF"), bpos::fleet_3_1, bcol::basic} {}
    bool active = true;
    bool is_active() { return active; }
    void update_active(client_context& context);
    void _clicked(client_context& context);
};

struct soldier_on_button : button<soldier_on_button>
{
    soldier_on_button() : button<soldier_on_button>{11, std::string("SOLDIERS ON"), bpos::fleet_3_2, bcol::basic} {}
    bool active = true;
    bool is_active() { return active; }
    void update_active(client_context& context);
    void _clicked(client_context& context);
};

struct soldier_off_button : button<soldier_off_button>
{
    soldier_off_button() : button<soldier_off_button>{12, std::string("SOLDIERS OFF"), bpos::fleet_4_1, bcol::basic} {}
    bool active = true;
    bool is_active() { return active; }
    void update_active(client_context& context);
    void _clicked(client_context& context);
};

struct invade_button : button<invade_button>
{
    invade_button() : button<invade_button>{4, std::string("INVADE"), bpos::fleet_4_2, bcol::basic} {}
    bool active = true;
    bool is_active() { return active; }
    void update_active(client_context& context);
    void _clicked(client_context& context);
};

struct fleet_cancel_button : button<fleet_cancel_button>
{
    fleet_cancel_button() : button<fleet_cancel_button>{4, std::string("CANCEL"), bpos::fleet_5_1, bcol::basic} {}
    bool active = true;
    bool is_active() { return active; }
    void update_active(client_context& context);
    void _clicked(client_context& context);
};

using tech_menu_button = std::variant<std::unique_ptr<research_button>, std::unique_ptr<abort_research_button>>;
using fleet_button =
    std::variant<std::unique_ptr<jump_button>, std::unique_ptr<build_base_button>, std::unique_ptr<colonize_button>,
                 std::unique_ptr<civil_on_button>, std::unique_ptr<civil_off_button>,
                 std::unique_ptr<soldier_on_button>, std::unique_ptr<soldier_off_button>,
                 std::unique_ptr<invade_button>, std::unique_ptr<fleet_cancel_button>>;

void update_all_fleet_buttons(std::vector<fleet_button>& buttons, client_context& context);
void set_fleet_button_color(fleet_button& b, client_context& context);
#endif  // BUTTONS_H
