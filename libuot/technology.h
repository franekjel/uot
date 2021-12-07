#pragma once

#include <memory>
#include <string>
#include <vector>

struct Technology
{
    enum TechnologyType
    {
        None,
        HyperquantumPhysics,
        SpaceEngineering,
        Engineering,
        Spaceships,
    };

    unsigned int id;
    std::string name;
    std::string description;
    float cost;
    std::vector<TechnologyType> unlock;

    Technology(const int id, const std::string& name, const std::string& desc, const float cost,
               const std::vector<TechnologyType>& unlock)
        : id(id), name(name), description(desc), cost(cost), unlock(unlock)
    {
    }

    bool operator<(const Technology& t) const { return this->id < t.id; }
};

// all technologies
const std::map<Technology::TechnologyType, Technology> Technologies{
    {Technology::TechnologyType::HyperquantumPhysics,
     Technology(0, "Hyperquantum physics", "Allow contructing improved reactors", 100, {})},
    {Technology::TechnologyType::SpaceEngineering,
     Technology(0, "Space engineering", "Allow contructing improved ship hulls", 200, {})},
    {Technology::TechnologyType::Engineering,
     Technology(0, "Engineering", "Starting technology. Allows constructing base buildngs", 0,
                {Technology::TechnologyType::SpaceEngineering})},
    {Technology::TechnologyType::Spaceships,
     Technology(0, "Spaceships", "Starting technology. Allows constructing base spaceships and base modules", 0,
                {Technology::TechnologyType::HyperquantumPhysics})}};
