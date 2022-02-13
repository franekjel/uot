#include "fleet_utils.h"
#include <input_utilities.h>
#include "player.h"
#include "ship.h"

namespace iu = input_utilities;

bool has_jump_neighbor(client_context& context)
{
    auto gui = context.gui;

    if (!gui->current_fleet.has_value() || !gui->current_sector.has_value())
        return false;

    auto& f = gui->current_fleet.value();
    auto& s = gui->current_sector.value();
    for (const auto& neighbor : gui->current_sector.value()->neighbors)
    {
        if (((neighbor->position - s->position).normalized() - f->position).squaredLength() < Fleet::kNearValue)
            return true;
    }

    return false;
}

bool fleet_can_colonize(client_context& context)
{
    if (!context.gui->current_fleet.has_value() || !context.gui->current_sector.has_value())
        return false;
    auto& s = context.gui->current_sector.value();
    auto& f = context.gui->current_fleet.value();

    if (f->construction_points <= 0.0f || f->civilians < Fleet::kColonizationCost)
        return false;

    for (auto& o : s->objects)
    {
        Planet* p = dynamic_cast<Planet*>(o.second.get());
        if (p == nullptr || p->colony != nullptr)
            continue;

        if ((p->position - f->position).squaredLength() > Fleet::kNearValue)
            continue;

        if (context.getGameState().value->player->owned_resources[Resource::Metals] < p->colony_metal_cost)
            continue;

        return true;
    }

    return false;
}

bool fleet_can_build(client_context& context)
{
    if (!context.gui->current_fleet.has_value() || !context.gui->current_sector.has_value())
        return false;

    auto& s = context.gui->current_sector.value();
    auto& f = context.gui->current_fleet.value();

    if (f->construction_points <= 0.0f)
        return false;

    for (auto& o : s->objects)
    {
        InhabitableObject* p = dynamic_cast<InhabitableObject*>(o.second.get());
        if (p == nullptr || p->resource_deposit.empty())
            continue;

        if (p->base != nullptr && p->base->owner->id == context.player_id)
            continue;

        if ((p->position - f->position).squaredLength() > Fleet::kNearValue)
            continue;

        auto [labour_cost, base_cost] = p->CalcBaseCost();
        for (const auto& [res, count] : base_cost)
        {
            if (context.getGameState().value->player->owned_resources[res] < count)
                continue;
        }

        return true;
    }

    return false;
}

bool fleet_can_civils_off(client_context& context)
{
    if (!context.gui->current_fleet.has_value() || !context.gui->current_sector.has_value())
        return false;

    auto& s = context.gui->current_sector.value();
    auto& f = context.gui->current_fleet.value();

    if (f->civilians <= 0.0f)
        return false;

    for (auto& o : s->objects)
    {
        Planet* p = dynamic_cast<Planet*>(o.second.get());
        if (p == nullptr || p->colony == nullptr || p->colony->owner->id != context.getGameState().value->player->id)
            continue;

        if ((p->position - f->position).squaredLength() > Fleet::kNearValue)
            continue;

        return true;
    }

    return false;
}

bool fleet_can_civils_on(client_context& context)
{
    if (!context.gui->current_fleet.has_value() || !context.gui->current_sector.has_value())
        return false;

    auto& s = context.gui->current_sector.value();
    auto& f = context.gui->current_fleet.value();

    if (f->human_capacity - (f->civilians + f->soldiers) <= 0.0f)
        return false;

    for (auto& o : s->objects)
    {
        Planet* p = dynamic_cast<Planet*>(o.second.get());
        if (p == nullptr || p->colony == nullptr || p->colony->owner->id != context.getGameState().value->player->id)
            continue;

        if (p->colony->population <= 1.0f)
            continue;

        if ((p->position - f->position).squaredLength() > Fleet::kNearValue)
            continue;

        return true;
    }

    return false;
}

bool fleet_can_soldiers_off(client_context& context)
{
    if (!context.gui->current_fleet.has_value() || !context.gui->current_sector.has_value())
        return false;

    auto& s = context.gui->current_sector.value();
    auto& f = context.gui->current_fleet.value();

    if (f->soldiers <= 0.0f)
        return false;

    for (auto& o : s->objects)
    {
        Planet* p = dynamic_cast<Planet*>(o.second.get());
        if (p == nullptr || p->colony == nullptr || p->colony->owner->id != context.getGameState().value->player->id)
            continue;

        if ((p->position - f->position).squaredLength() > Fleet::kNearValue)
            continue;

        return true;
    }

    return false;
}

bool fleet_can_soldiers_on(client_context& context)
{
    if (!context.gui->current_fleet.has_value() || !context.gui->current_sector.has_value())
        return false;

    auto& s = context.gui->current_sector.value();
    auto& f = context.gui->current_fleet.value();

    if (f->human_capacity - (f->civilians + f->soldiers) <= 0.0f)
        return false;

    for (auto& o : s->objects)
    {
        Planet* p = dynamic_cast<Planet*>(o.second.get());
        if (p == nullptr || p->colony == nullptr || p->colony->owner->id != context.getGameState().value->player->id)
            continue;

        if (p->colony->soldiers <= 0.0f)
            continue;

        if ((p->position - f->position).squaredLength() > Fleet::kNearValue)
            continue;

        return true;
    }

    return false;
}

bool fleet_can_invade(client_context& context)
{
    if (!context.gui->current_fleet.has_value() || !context.gui->current_sector.has_value())
        return false;
    auto& s = context.gui->current_sector.value();
    auto& f = context.gui->current_fleet.value();

    for (auto& o : s->objects)
    {
        Planet* p = dynamic_cast<Planet*>(o.second.get());
        if (p == nullptr || p->colony == nullptr || p->colony->owner->id == context.getGameState().value->player->id)
            continue;

        if ((p->position - f->position).squaredLength() > Fleet::kNearValue)
            continue;

        return true;
    }

    return false;
}
