#include "resource.h"

std::map<Resource, float> operator+(std::map<Resource, float> a, const std::map<Resource, float>& b)
{
    for (const auto& p : b)
    {
        a[p.first] += p.second;
    }
    return a;
}

std::map<Resource, float>& operator+=(std::map<Resource, float>& a, const std::map<Resource, float>& b)
{
    for (const auto& p : b)
    {
        a[p.first] += p.second;
    }
    return a;
}

std::map<Resource, float> operator*(std::map<Resource, float> a, const float b)
{
    for (auto& p : a)
    {
        p.second *= b;
    }
    return a;
}

std::map<Resource, float> operator*(const float b, std::map<Resource, float> a) { return a * b; }
