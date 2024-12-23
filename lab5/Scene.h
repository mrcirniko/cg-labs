#ifndef SCENE_H
#define SCENE_H

#include "Sphere.h"
#include "Plane.h"
#include <vector>

struct Scene {
    std::vector<Sphere> spheres;
    std::vector<Plane> planes;

    void addSphere(const Sphere& sphere) {
        spheres.push_back(sphere);
    }

    void addPlane(const Plane& plane) {
        planes.push_back(plane);
    }
};

#endif // SCENE_H