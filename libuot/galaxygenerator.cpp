#include "galaxygenerator.h"

#include <random>

#include "common.h"

static constexpr float habitable_planet_base_chance = 0.01f;

static std::normal_distribution<> size_distribution{1.0, 0.15};

static const std::vector<Star::StarType> all_star_types = {Star::StarType::Sunny,      Star::StarType::BlueGiant,
                                                           Star::StarType::RedGiant,   Star::StarType::WhiteDwarf,
                                                           Star::StarType::BrownDwarf, Star::StarType::BlackHole};

Point GalaxyGenerator::PointOnCircle(float r)
{
    std::uniform_real_distribution<float> dist(-1.0, 1.0);
    Point p = Point(dist(gen), dist(gen));
    p *= r / sqrtf(p.squaredLength());
    return p;
}

bool too_close(std::vector<Point> all, Point b, const float Eps)
{
    for (const auto &p : all)
    {
        if ((b - p).squaredLength() <= Eps)
            return true;
    }
    return false;
}

std::vector<Point> GalaxyGenerator::GeneratePositionsInSector(const std::vector<Point> &existing_points, int count)
{
    std::uniform_real_distribution<float> dist(-1.0, 1.0);
    std::vector<Point> re;
    for (int i = 0; i < count; i++)
    {
        Point pos(0, 0);
        while (pos.squaredLength() >= 0.9f || too_close(existing_points, pos, 0.05) || too_close(re, pos, 0.05f))
            pos = Point(std::clamp(dist(gen), -1.0f, 1.0f), std::clamp(dist(gen), -1.0f, 1.0f));
        re.emplace_back(pos);
    }
    return re;
}

void GalaxyGenerator::GenerateBlackHoleSector(std::map<unsigned int, std::shared_ptr<SectorObject>> &sector_objects,
                                              std::mt19937 &gen, unsigned int sector_id)
{
    int cloud_num = std::discrete_distribution<>({6, 4, 2, 2, 1})(gen);

    std::vector<Point> clouds_positions = GeneratePositionsInSector({Point(0.0f, 0.0f)}, cloud_num);

    for (const auto &pos : clouds_positions)
    {
        unsigned int id = id_source++;
        sector_objects.insert({id, std::shared_ptr<SectorObject>(new InhabitableObject(
                                       SectorObject(id, pos, float(size_distribution(gen)), sector_id), {},
                                       InhabitableObject::ObjectType::DarkMatterCloud))});
    }
}

std::shared_ptr<SectorObject> GalaxyGenerator::GenerateInhabitable(const Point &pos, InhabitableObject::ObjectType type,
                                                                   std::discrete_distribution<> resources_distribution,
                                                                   unsigned int sector_id)
{
    Resource r = Resource::Metals;
    int resource = resources_distribution(gen);
    if (resource == 0)
    {
        return std::shared_ptr<SectorObject>(
            new InhabitableObject(SectorObject(id_source++, pos, float(size_distribution(gen)), sector_id), {}, type));
    }
    std::discrete_distribution<> dist({2.0, 1.0, 1.0, 0.2, 0.1, 0.1});
    int resource_count = dist(gen) + 1;
    switch (dist(gen))
    {
        case 1:
            r = Resource::Metals;
            break;
        case 2:
            r = Resource::Antimatter;
            break;
        case 3:
            r = Resource::RareMetals;
            break;
        case 4:
            r = Resource::Crystals;
            break;
        case 5:
            r = Resource::Polymers;
            break;
    }
    return std::shared_ptr<SectorObject>(new InhabitableObject(
        SectorObject(id_source++, pos, float(size_distribution(gen)), sector_id), {{r, resource_count}}, type));
}

std::shared_ptr<SectorObject> GalaxyGenerator::GenerateHabitablePlanet(const Point &p, unsigned int sector_id)
{
    std::map<PlanetaryFeatures::PlanetaryFeatureType, int> features;
    Planet::PlanetClimate climate;

    switch (std::discrete_distribution<>({1, 2, 1})(gen))
    {
        case 0:
            climate = Planet::PlanetClimate::Cold;
            features.insert({PlanetaryFeatures::ColdClimate, 1});
            break;
        case 1:
            climate = Planet::PlanetClimate::Temperate;
            features.insert({PlanetaryFeatures::TemperateClimate, 1});
            break;
        case 2:
            climate = Planet::PlanetClimate::Hot;
            features.insert({PlanetaryFeatures::HotClimate, 1});
            break;
    }

    const auto add_features = [&](const PlanetaryFeatures::PlanetaryFeatureType type, std::discrete_distribution<> dist)
    {
        const int count = dist(gen);
        if (count > 0)
            features.insert({type, count});
    };

    add_features(PlanetaryFeatures::MetalsDeposit, {0, 1, 1, 1, 1});
    add_features(PlanetaryFeatures::RareMetalsDeposit, {20, 10, 5, 5});
    add_features(PlanetaryFeatures::CrystalsDeposit, {20, 10, 5, 5});
    add_features(PlanetaryFeatures::FertileLands, {20, 10, 5, 5});
    add_features(PlanetaryFeatures::AncientRuins, {80, 5, 5, 5, 5});
    add_features(PlanetaryFeatures::AncientNanobotsDeposit, {80, 5, 5, 5, 5});

    return std::make_shared<Planet>(SectorObject(id_source++, p, float(size_distribution(gen)), sector_id), climate,
                                    features);
}

std::map<unsigned int, std::shared_ptr<SectorObject>> GalaxyGenerator::GenerateSectorObjects(unsigned int sector_id)
{
    std::map<unsigned int, std::shared_ptr<SectorObject>> sector_objects;

    Star::StarType sector_star_type = {all_star_types[std::discrete_distribution<>({40, 30, 10, 5, 1, 1})(gen)]};

    unsigned int id = id_source++;
    sector_objects.insert(
        {id, std::shared_ptr<SectorObject>(new Star(
                 SectorObject(id, Point(0.0f, 0.0f), float(size_distribution(gen)), sector_id), sector_star_type))});

    if (sector_star_type == Star::StarType::BlackHole)
    {
        GenerateBlackHoleSector(sector_objects, gen, sector_id);
        return sector_objects;
    }

    const int num_objects = std::discrete_distribution<>({1, 2, 5, 10, 5, 3, 1})(gen);
    const float habitable_chance = habitable_planet_base_chance * habitable_planet_chance_multipler;

    std::vector<Point> objects_positions = GeneratePositionsInSector({Point(0.0f, 0.0f)}, num_objects);

    for (const auto &pos : objects_positions)
    {
        switch (std::discrete_distribution<>({1, 3, 3})(gen))
        {
            case 0:
            {
                std::shared_ptr<SectorObject> o =
                    GenerateInhabitable(pos, InhabitableObject::ObjectType::Asteroid, {40, 20, 8, 1, 1, 1}, sector_id);
                sector_objects.insert({o->id, o});
                break;
            }
            case 1:
            {
                if (std::bernoulli_distribution(habitable_chance)(gen))
                {  // habitable
                    std::shared_ptr<SectorObject> o = GenerateHabitablePlanet(pos, sector_id);
                    sector_objects.insert({o->id, o});
                }
                else
                {
                    std::shared_ptr<SectorObject> o = GenerateInhabitable(
                        pos, InhabitableObject::ObjectType::InhabitablePlanet, {40, 20, 8, 1, 1, 1}, sector_id);
                    sector_objects.insert({o->id, o});
                }

                break;
            }
            case 2:
            {
                std::shared_ptr<SectorObject> o =
                    GenerateInhabitable(pos, InhabitableObject::ObjectType::GasGiant, {20, 0, 4, 0, 0, 1}, sector_id);
                sector_objects.insert({o->id, o});
                break;
            }
        }
    }
    return sector_objects;
}

GalaxyGenerator::GalaxyGenerator(int size, float habitable_planet_chance_multipler)
    : size(size), habitable_planet_chance_multipler(habitable_planet_chance_multipler)
{
    gen = std::mt19937(dev());
}

Galaxy GalaxyGenerator::Generate()
{
    Galaxy galaxy;
    std::vector<Point> sector_positions;
    // std::uniform_real_distribution<float> dist(-1.0, 1.0);

    std::normal_distribution<> dist{0.0, 0.3};

    for (unsigned int i = 0; i < size; i++)
    {
        Point pos(0, 0);
        while (pos.squaredLength() >= 1.0f || too_close(sector_positions, pos, 0.01))
            pos = Point(std::clamp(dist(gen), -1.0, 1.0), std::clamp(dist(gen), -1.0, 1.0));
        const std::map<unsigned int, std::shared_ptr<SectorObject>> sector_objects = GenerateSectorObjects(i);
        galaxy.sectors.insert({i, std::make_shared<Sector>(i, pos, sector_objects)});
        sector_positions.push_back(pos);
    }
    for (auto &[id1, s1] : galaxy.sectors)
    {
        std::vector<std::pair<float, std::shared_ptr<Sector>>> dists;
        dists.reserve(galaxy.sectors.size());
        for (const auto &[id2, s2] : galaxy.sectors)
        {
            if (id2 != id1)
                dists.emplace_back(std::make_pair((s1->position - s2->position).squaredLength(), s2));
        }
        std::sort(dists.begin(), dists.end());
        const int neighbors_count = std::discrete_distribution<>({1, 2, 1})(gen) + 1;
        for (int i = 0; i < neighbors_count; i++)
        {
            s1->neighbors.insert(dists[i].second);
            dists[i].second->neighbors.insert(s1);
        }
    }

    return galaxy;
}
