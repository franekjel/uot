#pragma once

#include "galaxy.h"

#include <random>

class GalaxyGenerator
{
    const int size;
    const float habitable_planet_chance_multipler;

    std::random_device dev;
    std::mt19937 gen;

    Point PointOnCircle(float r);
    std::vector<Point> GeneratePositionsInSector(const std::vector<Point> &existing_points, int count);
    void GenerateBlackHoleSector(std::map<unsigned int, std::shared_ptr<SectorObject>> &sector_objects,
                                 std::mt19937 &gen, unsigned int sector_id);
    std::shared_ptr<SectorObject> GenerateInhabitable(const Point &pos, InhabitableObject::ObjectType type,
                                                      std::discrete_distribution<> resources_distribution,
                                                      unsigned int sector_id);
    std::shared_ptr<SectorObject> GenerateHabitablePlanet(const Point &p, unsigned int sector_id);

    std::map<unsigned int, std::shared_ptr<SectorObject>> GenerateSectorObjects(unsigned int sector_id);

   public:
    GalaxyGenerator(int size, float habitable_planet_chance_multipler);
    Galaxy Generate();
};
