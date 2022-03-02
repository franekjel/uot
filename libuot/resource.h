#pragma once

#include <array>
#include <map>
#include <string_view>
#include <string>

enum class Resource
{
    // base
    Metals = 0,      // for all sorts of building, ships, modules
    Antimatter = 1,  // energy - to upkee building and modules

    // uncommon - used for better modules and building
    RareMetals = 2,
    Crystals = 3,
    Polymers = 4,

    // rare - for unique modules and building
    DarkMatter = 5,       // powerful weapons
    AncientNanobots = 6,  // hp regen modules, contruction
    AncientRelics = 7,    // for special projects
    Spatium = 8,          // for warp gates

    Food = 9,         // Food is special
    Technology = 10,  // Technology is also special
    Last = Technology,
};

constexpr std::array<std::string_view, 11> resourceNames = {"Metals",     "Antimatter",

                                                            "RareMetals", "Crystals",        "Polymers",

                                                            "DarkMatter", "AncientNanobots", "AncientRelics", "Spatium",

                                                            "Food",       "Technology"};

std::map<Resource, float> operator+(std::map<Resource, float> a, const std::map<Resource, float>& b);
std::map<Resource, float>& operator+=(std::map<Resource, float>& a, const std::map<Resource, float>& b);
std::map<Resource, float> operator*(std::map<Resource, float> a, const float b);
std::map<Resource, float> operator*(const float b, std::map<Resource, float> a);
