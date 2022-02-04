#ifndef RENDERING_H
#define RENDERING_H

#include <memory>
#include <variant>
#include "gui/buttons.h"

namespace rendering
{
struct render_menu_view;
struct render_universe_view;
struct render_sector_view;
struct render_planet_view;
struct render_tech_view;

template <typename T>
using sp = std::shared_ptr<T>;

typedef std::variant<sp<render_menu_view>, sp<render_universe_view>, sp<render_sector_view>, sp<render_planet_view>,
                     sp<render_tech_view>>
    view_t;

}  // namespace rendering

#endif
