#ifndef RENDERING_COMMON_H
#define RENDERING_COMMON_H
#include "texture.h"
#include "ui_button.h"

struct client_context;

namespace rendering
{
void render_background(const client_context& context);

void render_button_sprite(ui_button& button, client_context& context);

void render_planet_helper(const client_context& context, const float size_multiplier, const int x_off, const int y_off,
                          const texture_t& tex);

void render_resource_bar(const client_context& context);

}  // namespace rendering
#endif  // COMMON_RENDERING_H
