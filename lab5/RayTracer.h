#ifndef RAY_TRACER_H
#define RAY_TRACER_H

#include "Ray.h"
#include "Scene.h"
#include "Camera.h"

struct RayTracer {
    Camera camera;
    Scene scene;
    Vector3 backgroundColor;

    RayTracer(const Camera& camera, const Scene& scene, const Vector3& backgroundColor)
        : camera(camera), scene(scene), backgroundColor(backgroundColor) {}

    Vector3 trace(const Ray& ray, int depth) const {
        if (depth > 3) return backgroundColor; // Ограничение глубины рекурсии

        float closestT = std::numeric_limits<float>::max();
        const Sphere* closestSphere = nullptr;
        const Plane* closestPlane = nullptr;

        // Проверка пересечения с сферами
        for (const auto& sphere : scene.spheres) {
            float t;
            if (sphere.intersect(ray.origin, ray.direction, t) && t < closestT) {
                closestT = t;
                closestSphere = &sphere;
            }
        }

        // Проверка пересечения с плоскостями
        for (const auto& plane : scene.planes) {
            float t;
            if (plane.intersect(ray.origin, ray.direction, t) && t < closestT) {
                closestT = t;
                closestPlane = &plane;
            }
        }

        if (closestSphere) {
            Vector3 hitPoint = ray.origin + ray.direction * closestT;
            Vector3 normal = (hitPoint - closestSphere->center).normalize();
            Vector3 reflectionDir = ray.direction - normal * 2 * ray.direction.dot(normal);
            Ray reflectionRay(hitPoint, reflectionDir);
            Vector3 reflectionColor = trace(reflectionRay, depth + 1);
            return closestSphere->color * (1 - closestSphere->reflection) + reflectionColor * closestSphere->reflection;
        }

        if (closestPlane) {
            Vector3 hitPoint = ray.origin + ray.direction * closestT;
            Vector3 reflectionDir = ray.direction - closestPlane->normal * 2 * ray.direction.dot(closestPlane->normal);
            Ray reflectionRay(hitPoint, reflectionDir);
            Vector3 reflectionColor = trace(reflectionRay, depth + 1);
            return closestPlane->color * (1 - closestPlane->reflection) + reflectionColor * closestPlane->reflection;
        }

        return backgroundColor;
    }

    void render(int width, int height, const std::string& filename) const {
        std::vector<unsigned char> image(width * height * 3);

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                float u = (x + 0.5f) / width;
                float v = (y + 0.5f) / height;
                float aspectRatio = float(width) / height;
                float scale = std::tan(camera.fov * 0.5f * M_PI / 180.0f);

                Vector3 rayDir = camera.forward + camera.right * (2 * u - 1) * aspectRatio * scale + camera.up * (1 - 2 * v) * scale;
                Ray ray(camera.position, rayDir);

                Vector3 color = trace(ray, 0);
                image[(y * width + x) * 3 + 0] = static_cast<unsigned char>(std::min(color.x, 1.0f) * 255);
                image[(y * width + x) * 3 + 1] = static_cast<unsigned char>(std::min(color.y, 1.0f) * 255);
                image[(y * width + x) * 3 + 2] = static_cast<unsigned char>(std::min(color.z, 1.0f) * 255);
            }
        }

        // Сохранение изображения в файл
        stbi_write_png(filename.c_str(), width, height, 3, image.data(), width * 3);
    }
};

#endif // RAY_TRACER_H