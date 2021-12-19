#include "ship.h"

ShipDesign::ShipDesign(const unsigned int id, const std::string &name, const ShipHull &hull,
                       const std::set<Module> &front, const std::set<Module> &sides, const std::set<Module> &back,
                       const std::set<Module> &inside)
    : id(id), name(name), hull(hull), front(front), sides(sides), back(back), inside(inside)
{
    for (const auto &m : front)
    {
        cost += m.cost;
        upkeep += m.additional_upkeep;
        ship_size += m.size;
    }
    for (const auto &m : sides)
    {
        cost += m.cost;
        upkeep += m.additional_upkeep;
        ship_size += m.size;
    }
    for (const auto &m : back)
    {
        cost += m.cost;
        upkeep += m.additional_upkeep;
        ship_size += m.size;
    }
    for (const auto &m : inside)
    {
        cost += m.cost;
        upkeep += m.additional_upkeep;
        ship_size += m.size;
    }
    upkeep += percentage_cost_upkeep * cost;

}
