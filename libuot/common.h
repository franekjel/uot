#pragma once

struct Point
{
    float x, y;
    Point() = default;
    Point(const float x, const float y) : x(x), y(y) {}

    float dotProduct(const Point& p) const { return x * p.x + y * p.y; }

    float squaredLength() const { return x * x + y * y; }

    Point& operator-()
    {
        x = -x;
        y = -y;
        return *this;
    }

    bool operator==(const Point& b) { return x == b.x && y == b.y; }

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
