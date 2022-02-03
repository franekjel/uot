#ifndef RENDERING_COMMON_H
#define RENDERING_COMMON_H
#include "texture.h"

struct client_context;

namespace rendering
{
void render_background(const client_context& context);

void render_planet_helper(const client_context& context, const float size_multiplier, const int x_off, const int y_off,
                          const texture_t& tex);

void render_resource_bar(client_context& context);

void render_planet_owner(const client_context& context, const unsigned int owner_id, const float size_multiplier,
                         const int x_off, const int y_off, const texture_t& tex);
}  // namespace rendering
#endif  // COMMON_RENDERING_H
