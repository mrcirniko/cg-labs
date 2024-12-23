#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <limits>

// Структура для представления вектора
struct Vec {
    float x, y, z;
    Vec() : x(0), y(0), z(0) {}
    Vec(float x, float y, float z) : x(x), y(y), z(z) {}

    Vec operator+(const Vec& v) const {
        return Vec(x + v.x, y + v.y, z + v.z);
    }

    Vec operator-(const Vec& v) const {
        return Vec(x - v.x, y - v.y, z - v.z);
    }

    Vec operator*(float k) const {
        return Vec(x * k, y * k, z * k);
    }

    float dot(const Vec& v) const {
        return x * v.x + y * v.y + z * v.z;
    }

    Vec normalize() const {
        float len = std::sqrt(x*x + y*y + z*z);
        return Vec(x / len, y / len, z / len);
    }
};

// Структура для представления цвета
struct Color {
    sf::Uint8 r, g, b;
    Color() : r(0), g(0), b(0) {}
    Color(sf::Uint8 r, sf::Uint8 g, sf::Uint8 b) : r(r), g(g), b(b) {}

    Color operator*(float k) const {
        return Color(r * k, g * k, b * k);
    }

    Color operator+(const Color& c) const {
        return Color(r + c.r, g + c.g, b + c.b);
    }
};

// Структура для представления сферы
struct Sphere {
    Vec center;
    float radius;
    Color color;
    float reflection;
    Sphere(const Vec& center, float radius, const Color& color, float reflection)
        : center(center), radius(radius), color(color), reflection(reflection) {}
};

// Структура для представления источника света
struct Light {
    Vec position;
    float intensity;
    Light(const Vec& position, float intensity)
        : position(position), intensity(intensity) {}
};

// Функции для трассировки лучей
float intersectRaySphere(const Vec& origin, const Vec& direction, const Sphere& sphere) {
    Vec oc = origin - sphere.center;
    float a = direction.dot(direction);
    float b = 2.0f * oc.dot(direction);
    float c = oc.dot(oc) - sphere.radius * sphere.radius;
    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0) return std::numeric_limits<float>::infinity();
    return (-b - std::sqrt(discriminant)) / (2.0f * a);
}

Color traceRay(const Vec& origin, const Vec& direction, const std::vector<Sphere>& spheres, const std::vector<Light>& lights, int depth) {
    float t_min = std::numeric_limits<float>::infinity();
    const Sphere* closest_sphere = nullptr;

    for (const auto& sphere : spheres) {
        float t = intersectRaySphere(origin, direction, sphere);
        if (t < t_min) {
            t_min = t;
            closest_sphere = &sphere;
        }
    }

    if (!closest_sphere) return Color(0, 0, 0);

    Vec point = origin + direction * t_min;
    Vec normal = (point - closest_sphere->center).normalize();
    Color color = closest_sphere->color * 0.1f;

    for (const auto& light : lights) {
        Vec light_dir = (light.position - point).normalize();
        float intensity = std::max(0.0f, normal.dot(light_dir)) * light.intensity;
        color = color + closest_sphere->color * intensity;
    }

    if (depth <= 0 || closest_sphere->reflection <= 0) return color;

    Vec reflection_dir = direction - normal * 2.0f * direction.dot(normal);
    Color reflection_color = traceRay(point, reflection_dir, spheres, lights, depth - 1);
    return color * (1 - closest_sphere->reflection) + reflection_color * closest_sphere->reflection;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Ray Tracing");
    sf::Image image;
    image.create(800, 600);

    std::vector<Sphere> spheres = {
        Sphere(Vec(0, -1, 3), 1, Color(255, 0, 0), 0.4f),
        Sphere(Vec(2, 0, 4), 1, Color(0, 0, 255), 0.3f),
        Sphere(Vec(-2, 0, 4), 1, Color(0, 255, 0), 0.4f),
        Sphere(Vec(0, -5001, 0), 5000, Color(255, 255, 0), 0.5f),
    };

    std::vector<Light> lights = {
        Light(Vec(5, 5, 5), 1.0f),
    };

    Vec camera(0, 0, 0);
    float viewport_size = 1.0f;
    float projection_plane_z = 1.0f;
    int recursion_depth = 3;

    for (int x = 0; x < 800; x++) {
        for (int y = 0; y < 600; y++) {
            float vx = (x - 400) * viewport_size / 800.0f;
            float vy = (300 - y) * viewport_size / 600.0f;
            Vec direction = Vec(vx, vy, projection_plane_z).normalize();
            Color color = traceRay(camera, direction, spheres, lights, recursion_depth);
            image.setPixel(x, y, sf::Color(color.r, color.g, color.b));
        }
    }

    sf::Texture texture;
    texture.loadFromImage(image);
    sf::Sprite sprite(texture);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(sprite);
        window.display();
    }

    return 0;
}