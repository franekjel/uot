#include "resource.h"

std::map<Resource, float> operator+(std::map<Resource, float> a, const std::map<Resource, float>& b)
{
    for (const auto& p : b)
    {
        if (a.count(p.first) < 1)
            a[p.first] = 0.0f;
        a[p.first] += p.second;
    }
    return a;
}

std::map<Resource, float>& operator+=(std::map<Resource, float>& a, const std::map<Resource, float>& b)
{
    for (const auto& p : b)
    {
        if (a.count(p.first) < 1)
            a[p.first] = 0.0f;
        a[p.first] += p.second;
    }
    return a;
}

std::map<Resource, float> operator*(std::map<Resource, float> a, const float b)
{
    for (auto& p : a)
    {
        if (a.count(p.first) < 1)
            a[p.first] = 0.0f;
        p.second *= b;
    }
    return a;
}

std::map<Resource, float> operator*(const float b, std::map<Resource, float> a) { return a * b; }
