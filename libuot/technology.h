#pragma once

#include <memory>
#include <string>
#include <vector>

struct Technology
{
    unsigned int id;
    std::string name;
    std::string description;
    float cost;
    std::vector<Technology const*> unlock;

    Technology(const int id, const std::string& name, const std::string& desc, const float cost,
               const std::vector<const Technology*>& unlock)
        : id(id), name(name), description(desc), cost(cost), unlock(unlock)
    {
    }

    bool operator<(const Technology& t) const { return this->id < t.id; }
};

const Technology HyperquantumPhysics(0, "Hyperquantum physics", "Allow contructing improved reactors", 100, {});
const Technology SpaceEngineering(0, "Space engineering", "Allow contructing improved ship hulls", 200, {});

const Technology Engineering(0, "Engineering", "Starting technology. Allows constructing base buildngs", 0,
                             {&SpaceEngineering});
const Technology Spaceships(0, "Spaceships",
                            "Starting technology. Allows constructing base spaceships and base modules", 0,
                            {&HyperquantumPhysics});
