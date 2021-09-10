#pragma once

#include <map>

// TODO - more fancy names
enum class Resource
{
    // base
    Metals,            // for all sorts of building, ships, modules
    Antimatter,        // energy - to upkee building and modules
    Cryptocurrencies,  // money

    // uncommon - used for better modules and building
    RareMetals,
    Crystals,
    RESOURCE_UNCOMMON_3,

    // rare - for unique modules and building
    DarkMatter,
    AncientNanobots,
    RESOURCE_RARE_3,
    RESOURCE_RARE_4,
    RESOURCE_RARE_5,

    Food,  // Food is special
};

std::map<Resource, float> operator+(std::map<Resource, float> a, const std::map<Resource, float>& b);
std::map<Resource, float>& operator+=(std::map<Resource, float>& a, const std::map<Resource, float>& b);
std::map<Resource, float> operator*(std::map<Resource, float> a, const float b);
std::map<Resource, float> operator*(const float b, std::map<Resource, float> a);
