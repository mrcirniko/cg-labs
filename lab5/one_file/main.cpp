#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <vector>
#include <limits>

// Векторные операции
struct Vector3 {
    float x, y, z;

    Vector3() : x(0), y(0), z(0) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

    Vector3 operator+(const Vector3& v) const { return Vector3(x + v.x, y + v.y, z + v.z); }
    Vector3 operator-(const Vector3& v) const { return Vector3(x - v.x, y - v.y, z - v.z); }
    Vector3 operator*(float scalar) const { return Vector3(x * scalar, y * scalar, z * scalar); }
    Vector3 operator/(float scalar) const { return Vector3(x / scalar, y / scalar, z / scalar); }

    float dot(const Vector3& v) const { return x * v.x + y * v.y + z * v.z; }
    Vector3 cross(const Vector3& v) const { return Vector3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); }

    Vector3 normalize() const {
        float mag = std::sqrt(x * x + y * y + z * z);
        return Vector3(x / mag, y / mag, z / mag);
    }
};

// Примитив сферы
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

// Примитив плоскости
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

// Описание лучей
struct Ray {
    Vector3 origin;
    Vector3 direction;

    Ray(const Vector3& origin, const Vector3& direction)
        : origin(origin), direction(direction.normalize()) {}
};

// Описание камеры
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

    void rotate(float angleX, float angleY) {
        // Обновление углов поворота
        static float horizontalAngle = 0.0f;
        static float verticalAngle = 0.0f;

        horizontalAngle += angleY;
        verticalAngle += angleX;

        // Ограничение вертикального угла поворота
        if (verticalAngle > 1.5f) verticalAngle = 1.5f;
        if (verticalAngle < -1.5f) verticalAngle = -1.5f;

        // Обновление направления камеры
        forward.x = cos(verticalAngle) * sin(horizontalAngle);
        forward.y = sin(verticalAngle);
        forward.z = cos(verticalAngle) * cos(horizontalAngle);
        forward = forward.normalize();

        right = Vector3(sin(horizontalAngle - 3.14f / 2.0f), 0, cos(horizontalAngle - 3.14f / 2.0f)).normalize();
        up = right.cross(forward).normalize();
    }
};

// Описание сцены
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

// Алгоритм трассировки лучей
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

    void render(sf::Image &image) const {
        for (unsigned int y = 0; y < image.getSize().y; ++y) {
            for (unsigned int x = 0; x < image.getSize().x; ++x) {
                float u = (x + 0.5f) / image.getSize().x;
                float v = (y + 0.5f) / image.getSize().y;
                float aspectRatio = float(image.getSize().x) / image.getSize().y;
                float scale = std::tan(camera.fov * 0.5f * M_PI / 180.0f);

                Vector3 rayDir = camera.forward + camera.right * (2 * u - 1) * aspectRatio * scale + camera.up * (1 - 2 * v) * scale;
                Ray ray(camera.position, rayDir);

                Vector3 color = trace(ray, 0);
                image.setPixel(x, y, sf::Color(
                    static_cast<unsigned char>(std::min(color.x, 1.0f) * 255),
                    static_cast<unsigned char>(std::min(color.y, 1.0f) * 255),
                    static_cast<unsigned char>(std::min(color.z, 1.0f) * 255)
                ));
            }
        }
    }
};

int main() {
    // Настройка окна
    sf::RenderWindow window(sf::VideoMode(800, 600), "Ray Tracing in Real-Time");

    // Настройка камеры
    Camera camera(Vector3(0, 1, 5), Vector3(0, 1, 0), Vector3(0, 1, 0), 45.0f);

    // Настройка сцены
    Scene scene;
    Sphere sphere1(Vector3(-1, 1, 0), 1.0f, Vector3(1, 0, 0), 0.5f);
    Sphere sphere2(Vector3(1, 1, 0), 1.0f, Vector3(0, 0, 1), 0.5f);
    Plane plane(Vector3(0, 0, 0), Vector3(0, 1, 0), Vector3(0.8f, 0.8f, 0.8f), 0.5f);
    scene.addSphere(sphere1);
    scene.addSphere(sphere2);
    scene.addPlane(plane);

    // Настройка трассировщика лучей
    RayTracer rayTracer(camera, scene, Vector3(0.2f, 0.2f, 0.2f));

    // Создание изображения
    sf::Image image;
    image.create(window.getSize().x, window.getSize().y, sf::Color::Black);
    sf::Texture texture;
    sf::Sprite sprite;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Управление камерой с помощью стрелок
        bool cameraMoved = false;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            camera.rotate(-0.01f, 0.0f);
            cameraMoved = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            camera.rotate(0.01f, 0.0f);
            cameraMoved = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            camera.rotate(0.0f, -0.01f);
            cameraMoved = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            camera.rotate(0.0f, 0.01f);
            cameraMoved = true;
        }

        // Управление коэффициентом отражения
        bool reflectionChanged = false;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) {
            sphere1.reflection = std::min(1.0f, sphere1.reflection + 0.01f);
            reflectionChanged = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) {
            sphere1.reflection = std::max(0.0f, sphere1.reflection - 0.01f);
            reflectionChanged = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) {
            sphere2.reflection = std::min(1.0f, sphere2.reflection + 0.01f);
            reflectionChanged = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4)) {
            sphere2.reflection = std::max(0.0f, sphere2.reflection - 0.01f);
            reflectionChanged = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num5)) {
            plane.reflection = std::min(1.0f, plane.reflection + 0.01f);
            reflectionChanged = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num6)) {
            plane.reflection = std::max(0.0f, plane.reflection - 0.01f);
            reflectionChanged = true;
        }

        if (cameraMoved || reflectionChanged) {
            std::cout << "Rendering with updated parameters..." << std::endl;
            std::cout << "Camera Forward: (" << camera.forward.x << ", " << camera.forward.y << ", " << camera.forward.z << ")" << std::endl;
            std::cout << "Sphere1 Reflection: " << sphere1.reflection << std::endl;
            std::cout << "Sphere2 Reflection: " << sphere2.reflection << std::endl;
            std::cout << "Plane Reflection: " << plane.reflection << std::endl;

            rayTracer.render(image);
            texture.loadFromImage(image);
            sprite.setTexture(texture);

            window.clear();
            window.draw(sprite);
            window.display();
        }
    }

    return 0;
}