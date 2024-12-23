#ifndef CAMERA_H
#define CAMERA_H

#include "Vector3.h"

struct Camera {
    Vector3 position;
    Vector3 forward;
    Vector3 up;
    Vector3 right;
    float fov; // Поле зрения (в градусах)

    Camera(const Vector3& position, const Vector3& target, const Vector3& up, float fov)
        : position(position), forward((target - position).normalize()), up(up.normalize()), fov(fov) {
        right = forward.cross(up).normalize();
        this->up = right.cross(forward).normalize();
    }
};

#endif // CAMERA_H