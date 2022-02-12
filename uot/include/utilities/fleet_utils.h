#ifndef UOT_FLEET_UTILS_H
#define UOT_FLEET_UTILS_H

#include <client_context.h>
#include "sector.h"

bool has_jump_neighbor(client_context& context);
bool fleet_can_colonize(client_context& context);
bool fleet_can_build(client_context& context);
bool fleet_can_civils_on(client_context& context);
bool fleet_can_civils_off(client_context& context);
bool fleet_can_soldiers_on(client_context& context);
bool fleet_can_soldiers_off(client_context& context);
bool fleet_can_invade(client_context& context);

#endif  // UOT_FLEET_UTILS_H
