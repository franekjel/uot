#pragma once

#include "galaxy.h"

struct GalaxyGeneratorParameters
{
    int size;
    float habitable_planet_chance_multipler;
};

Galaxy GenerateGalaxy(const GalaxyGeneratorParameters& parameters);
