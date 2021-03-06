#include "ship.h"

ShipHull::ShipHull(const int sides_size, const int inside_size, const std::map<Resource, float> &cost,
                   const std::map<Resource, float> &additional_upkeep, const float hp, const float speed,
                   const float engines_energy_consumtion, const float crew, const float worker_weeks_cost,
                   const float warp_drive_energy, const int ship_aggro,
                   const std::vector<Technology::TechnologyType> required_technologies)
    : sides_size(sides_size),
      inside_size(inside_size),
      cost(cost),
      additional_upkeep(additional_upkeep),
      hp(hp),
      speed(speed),
      engines_energy_consumtion(engines_energy_consumtion),
      crew(crew),
      worker_weeks_cost(worker_weeks_cost),
      warp_drive_energy(warp_drive_energy),
      ship_aggro(ship_aggro),
      required_technologies(required_technologies)
{
}

ShipDesign::ShipDesign(const unsigned int id, const std::string &name, const ShipHull::Type hull_type,
                       const std::map<ModuleType, int> &sides, const std::map<ModuleType, int> &inside)
    : id(id), name(name), hull_type(hull_type), sides(sides), inside(inside)
{
    auto &hull = ShipHulls.at(hull_type);
    worker_weeks_cost = hull.worker_weeks_cost;
    cost = hull.cost;
    upkeep = hull.additional_upkeep;
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
                ship->ship_weapons[type] += w.attack_count * count;
            }
            if (type == ModuleType::NanobotsSelfRepairModule)
            {
                ship->hp_regen += Module::nanobots_hp_regen_amount * count;
            }
        }
    }
    const auto &hull = ShipHulls.at(design->hull_type);
    ship->max_hp += hull.hp;
    ship->engines_energy_consumtion = hull.engines_energy_consumtion;
    ship->speed = hull.speed;

    ship->ship_aggro = hull.ship_aggro;

    ship->hp = ship->max_hp;
    ship->energy = ship->max_energy / 2.0f;
    ship->shield = ship->max_shield / 2.0f;
    ship->soldiers = 0.0f;
    ship->civilians = 0.0f;

    return ship;
}

bool IsShipDesignCorrect(const ShipHull::Type hull_type, const std::map<ModuleType, int> &sides,
                         const std::map<ModuleType, int> &inside)
{
    float max_energy = 0.0f;
    float max_hp = 0.0f;

    for (const auto &modules : {sides, inside})
    {
        for (const auto &[type, count] : modules)
        {
            const auto &m = Modules.at(type);
            max_hp += m.additional_hp * static_cast<float>(count);
            max_energy += m.energy_capacity * static_cast<float>(count);
        }
    }

    const auto &hull = ShipHulls.at(hull_type);
    max_hp += hull.hp;

    return max_energy > 0.0f && max_hp > 0.0f;
}

bool IsShipDesignPossible(const ShipHull::Type hull_type, const unsigned int sides_size, const unsigned int inside_size)
{
    const auto &hull = ShipHulls.at(hull_type);
    return inside_size <= hull.inside_size && sides_size <= hull.sides_size;
}

void Ship::JumpShip() { warp_drive_charge = 0.0f; }

bool Ship::ChargeWarpDrive()
{
    float charge_left = full_warp_drive_charge - warp_drive_charge;
    if (energy >= charge_left)
    {
        energy -= charge_left;
        warp_drive_charge = full_warp_drive_charge;
    }
    else
    {
        warp_drive_charge += energy;
        energy = 0.0f;
    }
    return full_warp_drive_charge <= warp_drive_charge;
}

float Ship::GetShipSpeed()
{
    speed = (energy >= engines_energy_consumtion ? speed : speed * (energy / engines_energy_consumtion));
    return speed;
}

void Ship::RegenShip()
{
    energy += energy_regen;
    energy = std::min(energy, max_energy);
    hp += hp_regen;
    hp = std::min(hp, max_hp);
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
    for (auto it = ships.begin(), next_it = it; it != ships.end(); it = next_it)
    {
        next_it++;
        if (it->second->hp <= 0.0f)
        {
            location_sector->destroyed_ships.push_back({it->second->id, owner_id});
            ships.erase(it);
        }
    }

    for (const auto &[id, ship] : ships)
    {
    }

    if (ships.size() == 0)
    {
        empty_fleet = true;
        return;
    }

    fleet_aggro = 0;
    for (const auto &[id, ship] : ships)
    {
        ship->RegenShip();
        fleet_aggro += ship->ship_aggro;
    }

    for (const auto &[id, ship] : ships)
    {
        ship->ship_weapons_current_state.clear();
        float needed_energy = 0.0f;
        for (const auto &[type, count] : ship->ship_weapons)
        {
            if (fleet_weapons[type].second == fleet_weapons[type].first)
                continue;
            float part = count / (float)fleet_weapons[type].first;
            ship->ship_weapons_current_state[type] = (fleet_weapons[type].first - fleet_weapons[type].second) * part;
            needed_energy += ship->ship_weapons_current_state[type] * (Modules.at(type).energy_usage) /
                             Modules.at(type).weapon.value().attack_count;
        }
        if (needed_energy <= 0.0f)
        {
            ship->reload_modifier = -1.0f;
            continue;
        }
        ship->reload_modifier = std::clamp(ship->energy / needed_energy, 0.0f, 1.0f);
        ship->energy -= needed_energy * ship->reload_modifier;
    }
    for (const auto &[id, ship] : ships)
    {
        if (ship->reload_modifier <= 0.0f)
            continue;
        for (auto &[type, count] : ship->ship_weapons_current_state)
        {
            fleet_weapons[type].second += std::round(count * ship->reload_modifier);
            count -= count * ship->reload_modifier;
        }
    }

    MoveFleet();

    if (current_action == Action::WarpLoading)
    {
        bool is_ready_to_jump = true;
        for (const auto &[id, ship] : ships)
        {
            is_ready_to_jump &= ship->ChargeWarpDrive();
        }
        if (is_ready_to_jump)
        {
            location_sector->JumpFleet(id);
        }
    }
}

void Fleet::UpdateFleetSpeed()
{
    if (ships.size() == 0)
    {
        empty_fleet = true;
        return;
    }

    float minimum_speed = ships.begin()->second->GetShipSpeed();
    for (const auto &[id, ship] : ships)
    {
        float speed = ship->GetShipSpeed();
        if (speed < minimum_speed)
            minimum_speed = speed;
    }

    fleet_speed_per_turn = minimum_speed;
}

void Fleet::MoveFleet()
{
    if (current_action != Fleet::Action::None)
    {
        wanted_position = position;
        return;
    }
    if (std::isnan(wanted_position.x) || wanted_position == position)
        return;
    UpdateFleetSpeed();
    if (fleet_speed_per_turn <= 0.0f)
        return;
    auto movement_vec = wanted_position - position;
    float movement_length = std::sqrt(movement_vec.squaredLength());
    if (movement_length > fleet_speed_per_turn)
    {
        for (const auto &[id, ship] : ships)
        {
            ship->MoveShip(fleet_speed_per_turn);
        }
        movement_vec *= fleet_speed_per_turn / movement_length;
        position += movement_vec;
    }
    else
    {
        for (const auto &[id, ship] : ships)
        {
            ship->MoveShip(movement_length);
        }
        position += movement_vec;
    }
}

void Fleet::JumpFleet()
{
    for (const auto &[id, ship] : ships)
    {
        ship->JumpShip();
    }

    current_action = Action::None;
}

Fleet::Fleet(const unsigned int id, const std::shared_ptr<Sector> &location_sector, const Point &position,
             const unsigned int owner_id)
    : id(id), location_sector(location_sector), position(position), owner_id(owner_id)
{
    current_action = None;
    ships = {};
    fleet_weapons = {};
    soldiers = 0.0f;
    civilians = 0.0f;
    human_capacity = 0.0f;
    construction_points = 0.0f;
}

void Fleet::AddShipToFleet(const std::shared_ptr<Ship> &ship)
{
    soldiers += ship->soldiers;
    civilians += ship->civilians;
    human_capacity += ship->human_capacity;
    construction_points += ship->construction_points;
    fleet_aggro += ship->ship_aggro;
    ships.insert({ship->id, ship});
    for (const auto &[type, amount] : ship->ship_weapons)
    {
        fleet_weapons[type].first += amount;
        fleet_weapons[type].second += amount;
    }
}

void Fleet::CountDamage()
{
    float damage_to_deal;
    float damage_left_to_deal;

    bool ship_was_destroyed = false;
    bool any_ship_destroyed = false;

    float damage_sum = 0.0f;

    for (const auto &[dmg_type, dmg] : gained_damage)
    {
        damage_sum += dmg;
    }
    if (damage_sum > 0.0f)
    {
        location_sector->fleets_in_fight.push_back(location_sector->present_fleets[id]);
    }

    auto counting_damage = [&](std::shared_ptr<Ship> ship, Weapon::SpecialFeatures type)
    {
        if (gained_damage[type] <= 0.0f)
            return;

        damage_to_deal = gained_damage[type] / (ship->ship_aggro / (float)fleet_aggro);
        damage_left_to_deal = damage_to_deal;
        if (type != Weapon::SpecialFeatures::BypassShield)
        {
            if (type == Weapon::SpecialFeatures::ShieldDamageBonus)
                ship->shield -= damage_left_to_deal * Weapon::bonus_damage;
            else
                ship->shield -= damage_left_to_deal;
            damage_left_to_deal = 0.0f;
            if (ship->shield < 0.0f)
            {
                if (type == Weapon::SpecialFeatures::ShieldDamageBonus)
                    damage_left_to_deal = -(ship->shield / Weapon::bonus_damage);
                else
                    damage_left_to_deal = -(ship->shield);
                ship->shield = 0.0f;
            }
        }
        if (damage_left_to_deal > 0.0f)
        {
            if (type == Weapon::SpecialFeatures::HPDamageBonus)
                ship->hp -= damage_left_to_deal * Weapon::bonus_damage;
            else
                ship->hp -= damage_left_to_deal;
            damage_left_to_deal = 0.0f;
            if (ship->hp <= 0.0f)
            {
                if (type == Weapon::SpecialFeatures::HPDamageBonus)
                    damage_left_to_deal = -(ship->hp / Weapon::bonus_damage);
                else
                    damage_left_to_deal = -(ship->hp);
                ship->hp = 0.0f;
                ship_was_destroyed = true;
                any_ship_destroyed = true;
            }
        }
        gained_damage[type] -= damage_to_deal - damage_left_to_deal;
    };

    while (damage_sum > 0.0f && !empty_fleet)
    {
        for (const auto &[id, ship] : ships)
        {
            counting_damage(ship, Weapon::SpecialFeatures::ShieldDamageBonus);
            counting_damage(ship, Weapon::SpecialFeatures::None);
            counting_damage(ship, Weapon::SpecialFeatures::HPDamageBonus);
            counting_damage(ship, Weapon::SpecialFeatures::BypassShield);
        }

        while (ship_was_destroyed)
        {
            ship_was_destroyed = false;
            auto itr = ships.begin();
            while (itr != ships.end())
            {
                if (itr->second->hp <= 0.0f)
                {
                    auto &sh = itr->second;
                    civilians -= sh->civilians;
                    soldiers -= sh->soldiers;
                    human_capacity -= sh->human_capacity;
                    construction_points -= sh->construction_points;

                    fleet_aggro -= sh->ship_aggro;

                    for (const auto &[type, amount] : sh->ship_weapons)
                    {
                        fleet_weapons[type].first -= amount;
                    }

                    location_sector->destroyed_ships.push_back({sh->id, owner_id});

                    ships.erase(itr);
                    ship_was_destroyed = true;
                    any_ship_destroyed = true;
                    break;
                }
                itr++;
            }
        }

        if (ships.begin() == ships.end())
            empty_fleet = true;

        damage_sum = 0.0f;

        for (const auto &[dmg_type, dmg] : gained_damage)
        {
            damage_sum += dmg;
        }

        if (any_ship_destroyed)
            location_sector->fleets_changed.push_back({id, owner_id, civilians, soldiers});
    }
}

void Fleet::MoveCiviliansToColony(std::shared_ptr<Colony> colony)
{
    float civilians_left_to_move = 5.0f;
    bool changed = false;

    auto iter = ships.begin();
    while (civilians_left_to_move != 0.0f && iter != ships.end())
    {
        auto ship = *iter;
        float added_civ = std::min(ship.second->civilians, civilians_left_to_move);
        colony->population += added_civ;
        colony->population_changed = true;
        ship.second->civilians -= added_civ;
        civilians_left_to_move -= added_civ;
        civilians -= added_civ;
        iter++;
        changed = true;
    }

    if (changed)
        location_sector->fleets_changed.push_back({id, owner_id, civilians, soldiers});
}

void Fleet::MoveCiviliansFromColony(std::shared_ptr<Colony> colony)
{
    float civilians_left_to_move = 5.0f;
    bool changed = false;

    auto iter = ships.begin();
    while (civilians_left_to_move != 0.0f && iter != ships.end())
    {
        auto ship = *iter;
        float added_civ = std::min(std::min(colony->population - 1.0f, civilians_left_to_move),
                                   ship.second->human_capacity - (ship.second->soldiers + ship.second->civilians));
        colony->population -= added_civ;
        colony->population_changed = true;
        ship.second->civilians += added_civ;
        civilians_left_to_move -= added_civ;
        civilians += added_civ;
        iter++;
        changed = true;
    }

    if (changed)
        location_sector->fleets_changed.push_back({id, owner_id, civilians, soldiers});
}

void Fleet::MoveSoldiersToColony(std::shared_ptr<Colony> colony)
{
    float soldiers_left_to_move = 5.0f;
    bool changed = false;

    auto iter = ships.begin();
    while (soldiers_left_to_move != 0.0f && iter != ships.end())
    {
        auto ship = *iter;
        float added_sol = std::min(ship.second->soldiers, soldiers_left_to_move);
        colony->soldiers += added_sol;
        colony->soldiers_changed = true;
        ship.second->soldiers -= added_sol;
        soldiers_left_to_move -= added_sol;
        soldiers -= added_sol;
        iter++;
        changed = true;
    }

    if (changed)
        location_sector->fleets_changed.push_back({id, owner_id, civilians, soldiers});
}

void Fleet::MoveSoldiersFromColony(std::shared_ptr<Colony> colony)
{
    float soldiers_left_to_move = 5.0f;
    bool changed = false;

    auto iter = ships.begin();
    while (soldiers_left_to_move != 0.0f && iter != ships.end())
    {
        auto ship = *iter;
        float added_sol = std::min(std::min(colony->soldiers, soldiers_left_to_move),
                                   ship.second->human_capacity - (ship.second->soldiers + ship.second->civilians));
        colony->soldiers -= added_sol;
        colony->soldiers_changed = true;
        ship.second->soldiers += added_sol;
        soldiers_left_to_move -= added_sol;
        soldiers += added_sol;
        iter++;
        changed = true;
    }

    if (changed)
        location_sector->fleets_changed.push_back({id, owner_id, civilians, soldiers});
}

std::map<Resource, float> Fleet::GetUpkeepCost()
{
    std::map<Resource, float> cost;
    for (const auto &[id, ship] : ships)
    {
        cost += ship->design->upkeep;
    }
    return cost;
}

void Fleet::InvadeColony(std::shared_ptr<Colony> colony)
{
    if (current_action != Fleet::Action::None)
        return;
    current_action = Fleet::Action::Invade;
    invaded_colony = colony;
}

void Fleet::KillSoldiers(float number)
{
    float soldiers_left_kill = number;

    auto iter = ships.begin();
    while (soldiers_left_kill != 0.0f && iter != ships.end())
    {
        auto ship = *iter;
        float killed_sol = std::min(soldiers_left_kill, ship.second->soldiers);
        ship.second->soldiers -= killed_sol;
        soldiers_left_kill -= killed_sol;
        soldiers -= killed_sol;
        iter++;
    }
}
