#include "ship.h"

ShipHull::ShipHull(const int sides_size, const int inside_size, const std::map<Resource, float> &cost,
                   const std::map<Resource, float> &additional_upkeep, const float hp, const float speed,
                   const float engines_energy_consumtion, const float crew, const float worker_weeks_cost,
                   const float warp_drive_energy, const int ship_aggro)
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
      ship_aggro(ship_aggro)
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

    fleet_aggro = 0;
    for (const auto &ship : ships)
    {
        ship->RegenShip();
        fleet_aggro += ship->ship_aggro;
    }

    for (auto &[type, amount] : fleet_weapons)
    {
        amount.second = amount.first;
    }

    MoveFleet();

    if (current_action == Action::WarpLoading)
    {
        bool is_ready_to_jump = true;
        for (const auto &ship : ships)
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

void Fleet::JumpFleet()
{
    for (const auto &ship : ships)
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
    ships.push_back(ship);
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
        for (const auto &ship : ships)
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
                if ((*itr)->hp <= 0.0f)
                {
                    auto &sh = *itr;
                    civilians -= sh->civilians;
                    soldiers -= sh->soldiers;
                    human_capacity -= sh->human_capacity;
                    construction_points -= sh->construction_points;

                    fleet_aggro -= sh->ship_aggro;

                    for (const auto &[type, amount] : sh->ship_weapons)
                    {
                        fleet_weapons[type].first -= amount;
                    }

                    location_sector->destroyed_ships.push_back({owner_id, sh->id});

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
        float added_civ = std::min(ship->civilians, civilians_left_to_move);
        colony->population += added_civ;
        colony->population_changed = true;
        ship->civilians -= added_civ;
        civilians_left_to_move -= added_civ;
        civilians -= added_civ;
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
                                   ship->human_capacity - (ship->soldiers + ship->civilians));
        colony->population -= added_civ;
        colony->population_changed = true;
        ship->civilians += added_civ;
        civilians_left_to_move -= added_civ;
        civilians += added_civ;
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
        float added_sol = std::min(ship->soldiers, soldiers_left_to_move);
        colony->soldiers += added_sol;
        colony->soldiers_changed = true;
        ship->soldiers -= added_sol;
        soldiers_left_to_move -= added_sol;
        soldiers -= added_sol;
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
                                   ship->human_capacity - (ship->soldiers + ship->civilians));
        colony->soldiers -= added_sol;
        colony->soldiers_changed = true;
        ship->soldiers += added_sol;
        soldiers_left_to_move -= added_sol;
        soldiers += added_sol;
        changed = true;
    }

    if (changed)
        location_sector->fleets_changed.push_back({id, owner_id, civilians, soldiers});
}

std::map<Resource, float> Fleet::GetUpkeepCost()
{
    std::map<Resource, float> cost;
    for (const auto &ship : ships)
    {
        cost += ship->design->upkeep;
    }
    return cost;
}