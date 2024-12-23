
#ifndef PLANE_H
#define PLANE_H

#include "Vector3.h"

struct Plane {
    Vector3 point;
    Vector3 normal;
    Vector3 color;
    float reflection; // Коэффициент отражения

    Plane(const Vector3& point, const Vector3& normal, const Vector3& color, float reflection)
        : point(point), normal(normal.normalize()), color(color), reflection(reflection) {}

    bool intersect(const Vector3& rayOrigin, const Vector3& rayDir, float& t) const {
        float denom = normal.dot(rayDir);
        if (std::abs(denom) > 1e-6) {
            t = (point - rayOrigin).dot(normal) / denom;
            return (t >= 0);
        }
        return false;
    }
};

#endif // PLANE_H