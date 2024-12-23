#ifndef SPHERE_H
#define SPHERE_H

#include "Vector3.h"

struct Sphere {
    Vector3 center;
    float radius;
    Vector3 color;
    float reflection; // Коэффициент отражения

    Sphere(const Vector3& center, float radius, const Vector3& color, float reflection)
        : center(center), radius(radius), color(color), reflection(reflection) {}

    bool intersect(const Vector3& rayOrigin, const Vector3& rayDir, float& t) const {
        Vector3 oc = rayOrigin - center;
        float a = rayDir.dot(rayDir);
        float b = 2.0f * oc.dot(rayDir);
        float c = oc.dot(oc) - radius * radius;
        float discriminant = b * b - 4 * a * c;
        if (discriminant < 0) return false;
        t = (-b - std::sqrt(discriminant)) / (2.0f * a);
        return true;
    }
};

#endif // SPHERE_H