#include "galaxygenerator.h"

#include <random>

#include "common.h"

static unsigned int sector_object_id = 1;

static constexpr float habitable_planet_base_chance = 0.01f;

static std::normal_distribution<> size_distribution{1.0, 0.15};

// TODO: parameteterization for chances and distributions

static Point PointOnCircle(float r)
{
    std::random_device dev;
    std::mt19937 gen(dev());
    std::uniform_real_distribution<float> dist(-1.0, 1.0);
    Point p = Point(dist(gen), dist(gen));
    p *= r / sqrtf(p.squaredLength());
    return p;
}

static const std::vector<Star::StarType> all_star_types = {Star::StarType::Sunny,      Star::StarType::BlueGiant,
                                                           Star::StarType::RedGiant,   Star::StarType::WhiteDwarf,
                                                           Star::StarType::BrownDwarf, Star::StarType::BlackHole};

static void GenerateBlackHoleSector(std::set<std::shared_ptr<SectorObject>> &sector_objects, std::mt19937 &gen)
{
    std::discrete_distribution<> cloud_dist = std::discrete_distribution<>({12, 3, 1, 1});
    int cloud_num = cloud_dist(gen);
    for (int i = 0; i < cloud_num; i++)
    {
        Point pos = PointOnCircle(0.2f + (0.7f / float(cloud_num)) * float(i));
        sector_objects.insert(std::shared_ptr<SectorObject>(
            new InhabitableObject(SectorObject(sector_object_id++, pos, float(size_distribution(gen))), {},
                                  InhabitableObject::ObjectType::DarkMatterCloud)));
    }
}

static std::shared_ptr<SectorObject> GenerateInhabitable(const Point &pos, InhabitableObject::ObjectType type,
                                                         std::mt19937 &gen,
                                                         std::discrete_distribution<> resources_distribution)
{
    Resource r = Resource::Metals;
    int resource = resources_distribution(gen);
    if (resource == 0)
    {
        return std::shared_ptr<SectorObject>(
            new InhabitableObject(SectorObject(sector_object_id++, pos, float(size_distribution(gen))), {}, type));
    }
    std::discrete_distribution<> dist({1, 1, 1, 1});
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
        SectorObject(sector_object_id++, pos, float(size_distribution(gen))), {{r, resource_count}}, type));
}

static std::set<std::shared_ptr<SectorObject>> GenerateSectorObjects(const GalaxyGeneratorParameters &parameters)
{
    std::set<std::shared_ptr<SectorObject>> sector_objects;

    std::random_device dev;
    std::mt19937 gen(dev());
    std::discrete_distribution<> dist({40, 30, 10, 5, 1, 1});

    int star_type_idx = dist(gen);
    std::vector<Star::StarType> sector_star_types = {all_star_types[star_type_idx]};
    if (star_type_idx <= 3 && dist(gen) >= 3)
    {
        sector_star_types.push_back(all_star_types[std::clamp(dist(gen), 0, 3)]);
    }

    if (sector_star_types.size() == 1)
    {
        sector_objects.insert(std::shared_ptr<SectorObject>(
            new Star(SectorObject(sector_object_id++, Point(0.0f, 0.0f), float(size_distribution(gen))),
                     sector_star_types.front())));
    }
    else if (sector_star_types.size() == 2)
    {
        Point p = PointOnCircle(0.2);
        sector_objects.insert(std::shared_ptr<SectorObject>(
            new Star(SectorObject(sector_object_id++, p, float(size_distribution(gen))), sector_star_types.front())));
        sector_objects.insert(std::shared_ptr<SectorObject>(
            new Star(SectorObject(sector_object_id++, -p, float(size_distribution(gen))), sector_star_types.back())));
    }

    if (sector_star_types.front() == Star::StarType::BlackHole)
    {
        GenerateBlackHoleSector(sector_objects, gen);
        return sector_objects;
    }

    dist = std::discrete_distribution<>({1, 2, 5, 10, 10, 8, 5, 3, 1});  // chances for objects
    int num_objects = dist(gen);
    float habitable_chance = habitable_planet_base_chance * parameters.habitable_planet_chance_multipler;
    for (int i = 0; i < num_objects; i++)
    {
        const float r = 0.2f + (0.7f / float(num_objects)) * float(i);
        dist = std::discrete_distribution<>({1, 3, 3});  // asteroids belt, planet, gas giant
        switch (dist(gen))
        {
            case 0:
            {
                dist = std::discrete_distribution<>({1, 3, 3, 3, 1});
                const int asteroids_number = dist(gen) + 1;
                for (int j = 0; j < asteroids_number; j++)
                {
                    sector_objects.insert(GenerateInhabitable(PointOnCircle(r), InhabitableObject::ObjectType::Asteroid,
                                                              gen, {40, 20, 8, 1, 1, 1}));
                }
                break;
            }
            case 1:
            {
                std::bernoulli_distribution dist(habitable_chance);
                if (dist(gen))
                {  // habitable
                    sector_objects.insert(std::shared_ptr<SectorObject>(
                        new Planet(SectorObject(sector_object_id++, PointOnCircle(r), float(size_distribution(gen))),
                                   Planet::PlanetClimate::Temperate, {})));
                    // TODO: better planet generation
                }
                else
                {
                    sector_objects.insert(GenerateInhabitable(
                        PointOnCircle(r), InhabitableObject::ObjectType::InhabitablePlanet, gen, {40, 20, 8, 1, 1, 1}));
                }

                break;
            }
            case 2:
            {
                sector_objects.insert(GenerateInhabitable(PointOnCircle(r), InhabitableObject::ObjectType::GasGiant,
                                                          gen, {20, 0, 4, 0, 0, 1}));
                break;
            }
        }
    }
    return sector_objects;
}

bool too_close(std::vector<Point> all, Point b)
{
    for (const auto &p : all)
    {
        if ((b - p).squaredLength() <= 0.02)
            return true;
    }
    return false;
}

Galaxy GenerateGalaxy(const GalaxyGeneratorParameters &parameters)
{
    std::random_device dev;
    std::mt19937 gen(dev());
    std::uniform_real_distribution<float> dist(-1.0, 1.0);

    Galaxy galaxy;
    std::vector<Point> sector_positions;

    for (int i = 0; i < parameters.size; i++)
    {
        Point pos(0, 0);
        while (pos.squaredLength() >= 1.0f || too_close(sector_positions, pos))
            pos = Point(dist(gen), dist(gen));
        const std::set<std::shared_ptr<SectorObject>> sector_objects = GenerateSectorObjects(parameters);
        galaxy.sectors.insert(std::shared_ptr<Sector>(new Sector{i, pos, {}, sector_objects}));
        sector_positions.push_back(pos);

        // TODO: neighbour find - brute force or use kd-trees (nanoflann?)
    }
    return galaxy;
}

Galaxy GenerateGalaxyTest(const GalaxyGeneratorParameters &parameters)
{
    std::random_device dev;
    std::mt19937 gen(dev());
    std::uniform_real_distribution<float> dist(-1.0, 1.0);

    Galaxy galaxy;

    for (int i = 0; i < parameters.size; i++)
    {
        Point pos(dist(gen), dist(gen));
        // TODO: check if new sector isn't too close to other secotr

        const std::set<std::shared_ptr<SectorObject>> sector_objects = GenerateSectorObjects(parameters);
        galaxy.sectors.insert(std::shared_ptr<Sector>(new Sector{i, pos, {}, sector_objects}));
        // TODO: neighbour find - brute force or use kd-trees (nanoflann?)
    }
    for (auto sector1 : galaxy.sectors)
        for (auto sector2 : galaxy.sectors)
        {
            if (sector1->sector_id + 1 == sector2->sector_id)
            {
                sector1->neighbors.insert(sector2);
                sector2->neighbors.insert(sector1);
            }
        }
    return galaxy;
}
