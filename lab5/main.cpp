#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "RayTracer.h"
#include <iostream>

int main() {
    // Настройка камеры
    Camera camera(Vector3(0, 1, 5), Vector3(0, 1, 0), Vector3(0, 1, 0), 45.0f);

    // Настройка сцены
    Scene scene;
    scene.addSphere(Sphere(Vector3(-1, 1, 0), 1.0f, Vector3(1, 0, 0), 0.5f));
    scene.addSphere(Sphere(Vector3(1, 1, 0), 1.0f, Vector3(0, 0, 1), 0.5f));
    scene.addPlane(Plane(Vector3(0, 0, 0), Vector3(0, 1, 0), Vector3(0.8f, 0.8f, 0.8f), 0.5f));

    // Настройка трассировщика лучей
    RayTracer rayTracer(camera, scene, Vector3(0.2f, 0.2f, 0.2f));

    // Рендеринг сцены
    rayTracer.render(800, 600, "output.png");

    std::cout << "Rendering completed. Image saved as output.png" << std::endl;

    return 0;
}