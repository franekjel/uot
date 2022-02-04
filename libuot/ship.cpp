#include "ship.h"

ShipHull::ShipHull(const int sides_size, const int inside_size, const std::map<Resource, float> &cost,
                   const std::map<Resource, float> &additional_upkeep, const float hp, const float engines_power,
                   const float engines_energy_consumtion, const float crew, const float worker_weeks_cost,
                   const float warp_drive_energy)
    : sides_size(sides_size),
      inside_size(inside_size),
      cost(cost),
      additional_upkeep(additional_upkeep),
      hp(hp),
      speed(engines_power),
      engines_energy_consumtion(engines_energy_consumtion),
      crew(crew),
      worker_weeks_cost(worker_weeks_cost),
      warp_drive_energy(warp_drive_energy)
{
}

ShipDesign::ShipDesign(const unsigned int id, const std::string &name, const ShipHull *hull,
                       const std::map<ModuleType, int> &sides, const std::map<ModuleType, int> &inside)
    : id(id), name(name), hull(hull), sides(sides), inside(inside)
{
    worker_weeks_cost = hull->worker_weeks_cost;
    cost = hull->cost;
    upkeep = hull->additional_upkeep;
    for (const auto &module : {sides, inside})
    {
        for (const auto [type, count] : module)
        {
            const auto &m = Modules.at(type);
            cost += m.cost * count;
            upkeep += m.additional_upkeep * count;
            worker_weeks_cost += Module::worker_weeks_cost_per_size * m.size * count;
        }
    }
    upkeep += percentage_cost_upkeep * cost;
}

std::shared_ptr<Ship> Ship::ShipFromDesign(const int id, const std::shared_ptr<ShipDesign> design)
{
    auto ship = std::make_shared<Ship>();
    ship->id = id;
    ship->design = design;
    for (const auto &modules : {design->sides, design->inside})
    {
        for (const auto &[type, count] : modules)
        {
            const auto &m = Modules.at(type);
            ship->max_hp += m.additional_hp * count;
            ship->max_shield += m.shield_capacity * count;
            ship->max_energy += m.energy_capacity * count;
            ship->energy_regen += m.generating_energy * count;
            ship->human_capacity += m.human_capacity * count;
            ship->construction_points += m.contruction_speed * count;
            if (m.weapon.has_value())
            {
                Weapon w = m.weapon.value();
                w.attack_count = w.attack_count * count;
                ship->weapons.push_back(w);
            }
            if (type == ModuleType::NanobotsSelfRepairModule)
            {
                ship->hp_regen += Module::nanobots_hp_regen_amount * count;
            }
        }
    }
    ship->max_hp += design->hull->hp;
    ship->engines_energy_consumtion = design->hull->engines_energy_consumtion;
    ship->speed = design->hull->speed;

    ship->hp = ship->max_hp;
    ship->energy = ship->max_energy / 2.0f;
    ship->shield = ship->max_shield / 2.0f;
    ship->soldiers = 0.0f;
    ship->civilians = 0.0f;

    return ship;
}

float Ship::GetShipSpeed()
{
    speed = (energy >= engines_energy_consumtion ? speed : speed * (energy / engines_energy_consumtion));
    return speed;
}

void Ship::RegenShip()
{
    energy += energy_regen;
    energy = std::max(energy, max_energy);
    hp += hp_regen;
    hp = std::max(hp, max_hp);
    float shield_regeneration = std::min(max_shield - shield, max_shield * shield_regen_percentage);
    float shield_regen_cost = shield_regeneration * shield_regen_energy_cost;
    if (energy >= shield_regen_cost)
    {
        shield += shield_regeneration;
        energy -= shield_regen_cost;
    }
    else
    {
        shield += energy * shield_regen_energy_cost_reverse;
        energy = 0.0f;
    }
}

void Fleet::UpdateFleet()
{
    int to_delete = -1;
    do
    {
        to_delete = -1;
        for (int i = 0; i < ships.size(); i++)
        {
            if (ships[i]->hp <= 0)
            {
                to_delete = i;
                break;
            }
        }
        if (to_delete >= 0)
        {
            ships.erase(ships.begin() + to_delete);
        }
    } while (to_delete >= 0);

    if (ships.size() == 0)
    {
        empty_fleet = true;
        return;
    }

    for (const auto &ship : ships)
    {
        ship->RegenShip();
    }

    MoveFleet();
}

void Fleet::UpdateFleetSpeed()
{
    if (ships.size() == 0)
    {
        empty_fleet = true;
        return;
    }

    float minimum_speed = ships.front()->GetShipSpeed();
    for (const auto &ship : ships)
    {
        float speed = ship->GetShipSpeed();
        if (speed < minimum_speed)
            minimum_speed = speed;
    }

    fleet_speed_per_turn = minimum_speed;
}

void Fleet::MoveFleet()
{
    if (std::isnan(wanted_position.x) || wanted_position == position)
        return;
    UpdateFleetSpeed();
    if (fleet_speed_per_turn <= 0.0f)
        return;
    auto movement_vec = position - wanted_position;
    float movement_length = std::sqrt(movement_vec.squaredLength());
    if (movement_length > fleet_speed_per_turn)
    {
        for (const auto &ship : ships)
        {
            ship->MoveShip(fleet_speed_per_turn);
        }
        movement_vec *= fleet_speed_per_turn / movement_length;
        position += movement_vec;
    }
    else
    {
        for (const auto &ship : ships)
        {
            ship->MoveShip(movement_length);
        }
        position += movement_vec;
    }
}
