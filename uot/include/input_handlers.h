#ifndef INPUT_HANDLERS_H
#define INPUT_HANDLERS_H

#include "client_context.h"
#include "sdl_utilities.h"

namespace input_handlers
{
void handleMouse(client_context& context, Uint32 event_type, SDL_MouseButtonEvent m, int x, int y);
void handle_keydown(client_context& context, Uint16 k);
}

#endif