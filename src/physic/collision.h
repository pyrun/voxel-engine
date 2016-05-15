#ifndef COLLISION_H
#define COLLISION_H

#include <assert.h>
#include <math.h>

struct Point
{
    Point() {}
    Point( double x, double y, double z )
        : x(x)
        , y(y)
        , z(z)
    {}
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
    double w = 0.0;

    const double operator[](const int idx) const
    {
        if (idx == 0) return x;
        if (idx == 1) return y;
        if (idx == 2) return z;
        if (idx == 3) return w;

        assert(0);
    }
};

struct AABB {
    AABB() : c(), r() {}

    AABB(const Point & center, const Point & halfwidths)
        : c(center)
        , r(halfwidths)
    {}

    Point c;        // center point
    Point r;        // halfwidths
};

double Abs(double a);
bool testAABBAABB(const AABB &a, const AABB &b);



#endif // COLLISION_H
