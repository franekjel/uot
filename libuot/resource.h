#pragma once

#include <array>
#include <map>
#include <string>

enum class Resource
{
    // base
    Metals,      // for all sorts of building, ships, modules
    Antimatter,  // energy - to upkee building and modules

    // uncommon - used for better modules and building
    RareMetals,
    Crystals,
    Polymers,

    // rare - for unique modules and building
    DarkMatter,       // powerful weapons
    AncientNanobots,  // hp regen modules, contruction
    AncientRelics,    // for special projects
    Spatium,          // for warp gates

    Food,        // Food is special
    Technology,  // Technology is also special
};

constexpr std::array<std::string_view, 11> resourceNames = {"Metals",     "Antimatter",

                                                            "RareMetals", "Crystals",        "Polymers",

                                                            "DarkMatter", "AncientNanobots", "AncientRelics", "Spatium",

                                                            "Food",       "Technology"};

std::map<Resource, float> operator+(std::map<Resource, float> a, const std::map<Resource, float>& b);
std::map<Resource, float>& operator+=(std::map<Resource, float>& a, const std::map<Resource, float>& b);
std::map<Resource, float> operator*(std::map<Resource, float> a, const float b);
std::map<Resource, float> operator*(const float b, std::map<Resource, float> a);
