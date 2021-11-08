#pragma once

#include <map>

enum class Resource
{
    // base
    Metals,            // for all sorts of building, ships, modules
    Antimatter,        // energy - to upkee building and modules
    Cryptocurrencies,  // money

    // uncommon - used for better modules and building
    RareMetals,
    Crystals,
    Polymers,

    // rare - for unique modules and building
    DarkMatter,       // powerful weapons
    AncientNanobots,  // hp regen modules, contruction
    AncientRelics,    // for special projects
    Spatium,          // for warp gates

    Food,  // Food is special
};

std::map<Resource, float> operator+(std::map<Resource, float> a, const std::map<Resource, float>& b);
std::map<Resource, float>& operator+=(std::map<Resource, float>& a, const std::map<Resource, float>& b);
std::map<Resource, float> operator*(std::map<Resource, float> a, const float b);
std::map<Resource, float> operator*(const float b, std::map<Resource, float> a);
