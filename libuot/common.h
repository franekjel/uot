#pragma once

struct Point
{
    float x, y;
    Point(const float x, const float y) : x(x), y(y) {}

    float dotProduct(const Point& p) { return x * p.x + y * p.y; }

    float squaredLength() { return x * x + y * y; }

    Point& operator-()
    {
        x = -x;
        y = -y;
        return *this;
    }

    Point& operator+=(const Point& p)
    {
        x += p.x;
        y += p.y;
        return *this;
    }

    Point& operator-=(const Point& p)
    {
        x -= p.x;
        y -= p.y;
        return *this;
    }

    Point& operator*=(const float v)
    {
        x *= v;
        y *= v;
        return *this;
    }
};

inline Point operator+(Point lhs, const Point& rhs)
{
    lhs += rhs;
    return lhs;
}

inline Point operator-(Point lhs, const Point& rhs)
{
    lhs -= rhs;
    return lhs;
}

inline Point operator*(Point lhs, const float rhs)
{
    lhs *= rhs;
    return lhs;
}
