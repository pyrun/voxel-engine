#include "collision.h"

double Abs(double a)
{
    return fabs(a);
}

bool testAABBAABB(const AABB &a, const AABB &b)
{
    if ( Abs(a.c[0] - b.c[0]) > (a.r[0] + b.r[0]) ) return false;
    if ( Abs(a.c[1] - b.c[1]) > (a.r[1] + b.r[1]) ) return false;
    if ( Abs(a.c[2] - b.c[2]) > (a.r[2] + b.r[2]) ) return false;

    // We have an overlap
    return true;
};
