#ifndef RAY_H
#define RAY_H

#include "Vector3.h"

struct Ray {
    Vector3 origin;
    Vector3 direction;

    Ray(const Vector3& origin, const Vector3& direction)
        : origin(origin), direction(direction.normalize()) {}
};

#endif // RAY_H