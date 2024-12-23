#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <cmath>

using namespace glm;

struct Vec3 {
    float x, y, z;
    Vec3 operator+(const Vec3& v) const { return {x + v.x, y + v.y, z + v.z}; }
    Vec3 operator-(const Vec3& v) const { return {x - v.x, y - v.y, z - v.z}; }
    Vec3 operator*(float f) const { return {x * f, y * f, z * f}; }
    Vec3 operator/(float f) const { return {x / f, y / f, z / f}; }
    float dot(const Vec3& v) const { return x * v.x + y * v.y + z * v.z; }
    Vec3 normalize() const {
        float len = std::sqrt(x * x + y * y + z * z);
        return {x / len, y / len, z / len};
    }
};

struct Ray {
    Vec3 origin, direction;
};

struct Sphere {
    Vec3 center;
    float radius;
    Vec3 color;
    float reflection;
    bool intersect(const Ray& ray, float& t) const {
        Vec3 oc = ray.origin - center;
        float a = ray.direction.dot(ray.direction);
        float b = 2.0f * oc.dot(ray.direction);
        float c = oc.dot(oc) - radius * radius;
        float discriminant = b * b - 4 * a * c;
        if (discriminant > 0) {
            t = (-b - std::sqrt(discriminant)) / (2.0f * a);
            return t > 0;
        }
        return false;
    }
};

struct Plane {
    Vec3 point, normal;
    Vec3 color;
    float reflection;
    bool intersect(const Ray& ray, float& t) const {
        float denom = normal.dot(ray.direction);
        if (std::fabs(denom) > 1e-6) {
            t = (point - ray.origin).dot(normal) / denom;
            return t >= 0;
        }
        return false;
    }
};

Vec3 trace(const Ray& ray, const std::vector<Sphere>& spheres, const Plane& plane, int depth) {
    if (depth > 3) return {0, 0, 0};

    float t = 1e6;
    const Sphere* hitSphere = nullptr;
    for (const auto& sphere : spheres) {
        float tSphere;
        if (sphere.intersect(ray, tSphere) && tSphere < t) {
            t = tSphere;
            hitSphere = &sphere;
        }
    }

    float tPlane;
    bool hitPlane = plane.intersect(ray, tPlane);
    if (hitPlane && tPlane < t) {
        t = tPlane;
        hitSphere = nullptr;
    }

    if (!hitSphere && !hitPlane) return {0.2f, 0.7f, 0.8f};

    Vec3 hitPoint = ray.origin + ray.direction * t;
    Vec3 normal;
    Vec3 color;
    float reflection;

    if (hitSphere) {
        normal = (hitPoint - hitSphere->center).normalize();
        color = hitSphere->color;
        reflection = hitSphere->reflection;
    } else {
        normal = plane.normal;
        color = plane.color;
        reflection = plane.reflection;
    }

    Vec3 reflectionDir = ray.direction - normal * 2.0f * ray.direction.dot(normal);
    Ray reflectionRay = {hitPoint + normal * 1e-4f, reflectionDir.normalize()};

    Vec3 reflectionColor = trace(reflectionRay, spheres, plane, depth + 1);
    return color * (1 - reflection) + reflectionColor * reflection;
}

int main() {
    sf::Window window(sf::VideoMode(800, 600), "Ray Tracing", sf::Style::Default, sf::ContextSettings(32));
    window.setVerticalSyncEnabled(true);

    std::vector<Sphere> spheres = {
        {{0.0f, -1.0f, -5.0f}, 1.0f, {0.7f, 0.3f, 0.3f}, 0.5f}
    };
    Plane plane = {{0.0f, -2.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.5f, 0.5f, 0.5f}, 0.1f};

    // Initialize OpenGL projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 800.0 / 600.0, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        gluLookAt(0, 0, 0, 0, 0, -1, 0, 1, 0);

        glBegin(GL_POINTS);
        for (int y = 0; y < 600; ++y) {
            for (int x = 0; x < 800; ++x) {
                float u = (x / 800.0f) * 2.0f - 1.0f;
                float v = (y / 600.0f) * 2.0f - 1.0f;
                Ray ray = {{0, 0, 0}, Vec3{u, v, -1}.normalize()};
                Vec3 color = trace(ray, spheres, plane, 0);
                glColor3f(color.x, color.y, color.z);
                glVertex2i(x, 600 - y);  // Flip y-coordinate
            }
        }
        glEnd();

        window.display();
    }

    return 0;
}