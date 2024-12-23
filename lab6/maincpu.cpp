#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>
#include <limits>
#include <cmath>

using namespace glm;

struct Ray {
    vec3 origin;
    vec3 direction;
};

struct Sphere {
    vec3 center;
    float radius;
    vec3 color;
    float reflectivity;
};

struct Plane {
    vec3 point;
    vec3 normal;
    vec3 color;
    float reflectivity;
};

struct Light {
    vec3 position;
    vec3 color;
};

// Intersect ray with sphere
bool intersectSphere(const Ray& ray, const Sphere& sphere, float& t) {
    vec3 oc = ray.origin - sphere.center;
    float a = dot(ray.direction, ray.direction);
    float b = 2.0f * dot(oc, ray.direction);
    float c = dot(oc, oc) - sphere.radius * sphere.radius;
    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0) return false;
    t = (-b - sqrt(discriminant)) / (2.0f * a);
    return t > 0;
}

// Intersect ray with plane
bool intersectPlane(const Ray& ray, const Plane& plane, float& t) {
    float denom = dot(plane.normal, ray.direction);
    if (abs(denom) > 1e-6) {
        t = dot(plane.point - ray.origin, plane.normal) / denom;
        return t > 0;
    }
    return false;
}

vec3 trace(const Ray& ray, const std::vector<Sphere>& spheres, const Plane& plane, const Light& light, int depth) {
    const int maxDepth = 3;
    if (depth > maxDepth) return vec3(0);

    float tSphere = std::numeric_limits<float>::max(), tPlane = std::numeric_limits<float>::max();
    const Sphere* hitSphere = nullptr;
    for (const auto& sphere : spheres) {
        float t;
        if (intersectSphere(ray, sphere, t) && t < tSphere) {
            tSphere = t;
            hitSphere = &sphere;
        }
    }

    float t;
    if (intersectPlane(ray, plane, t)) tPlane = t;

    if (!hitSphere && tPlane == std::numeric_limits<float>::max()) return vec3(0.1); // Background color

    vec3 hitColor;
    vec3 hitPoint;
    vec3 normal;
    float reflectivity;

    if (tSphere < tPlane) {
        hitPoint = ray.origin + ray.direction * tSphere;
        normal = normalize(hitPoint - hitSphere->center);
        hitColor = hitSphere->color;
        reflectivity = hitSphere->reflectivity;
        //std::cout << "Hit sphere at (" << hitPoint.x << ", " << hitPoint.y << ", " << hitPoint.z << ")\n";
    } else {
        hitPoint = ray.origin + ray.direction * tPlane;
        normal = plane.normal;
        hitColor = plane.color;
        reflectivity = plane.reflectivity;
        //std::cout << "Hit plane at (" << hitPoint.x << ", " << hitPoint.y << ", " << hitPoint.z << ")\n";
    }

    // Calculate lighting (diffuse and specular)
    vec3 lightDir = normalize(light.position - hitPoint);
    float diffuse = max(dot(normal, lightDir), 0.0f);

    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 viewDir = normalize(-ray.direction);
    float specular = pow(max(dot(viewDir, reflectDir), 0.0f), 16);

    // Calculate reflection
    vec3 reflectedColor(0);
    if (reflectivity > 0.0f) {
        Ray reflectedRay;
        reflectedRay.origin = hitPoint + normal * 1e-4f; // Avoid self-intersection
        reflectedRay.direction = reflect(ray.direction, normal);
        reflectedColor = trace(reflectedRay, spheres, plane, light, depth + 1);
    }

    // Final color calculation
    vec3 ambient = vec3(0.1);
    vec3 finalColor = ambient + (diffuse + specular) * hitColor + reflectedColor * reflectivity;
    return clamp(finalColor, 0.0f, 1.0f);
}

int main() {
    const int width = 800;
    const int height = 600;

    sf::RenderWindow window(sf::VideoMode(width, height), "Ray Tracing", sf::Style::Close);
    window.setVerticalSyncEnabled(true);

    sf::Image image;
    image.create(width, height);

    std::vector<Sphere> spheres = {
        {{-1.5, 0, -5}, 1, {1, 0, 0}, 0.5f}, // Красная сфера с 50% отражением
        {{1.5, 0, -4}, 1, {0, 0, 1}, 0.8f}  // Синяя сфера с 80% отражением
    };

    Plane plane = {{0, -1, 0}, {0, 1, 0}, {0.5, 0.5, 0.5}, 0.3f}; // Пол с 30% отражением
    Light light = {{2, 5, -3}, {1, 1, 1}};

    vec3 cameraPos = vec3(0, 2, 5);
    vec3 cameraFront = vec3(0, 0, -1);
    vec3 cameraUp = vec3(0, 1, 0);
    float cameraSpeed = 1.0f;
    float rotationSpeed = 0.05f;

    auto updateImage = [&]() {
        mat4 view = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                float u = (2.0f * x - width) / height;
                float v = (2.0f * y - height) / height;

                vec3 direction = normalize(vec3(u, v, -1));
                direction = vec3(inverse(view) * vec4(direction, 0.0f));

                Ray ray;
                ray.origin = cameraPos;
                ray.direction = normalize(direction);

                vec3 color = trace(ray, spheres, plane, light, 0);
                image.setPixel(x, y, sf::Color(color.r * 255, color.g * 255, color.b * 255));
            }
        }
    };

    updateImage();

    sf::Texture texture;
    texture.loadFromImage(image);
    sf::Sprite sprite(texture);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::W) {
                    cameraPos += cameraSpeed * cameraFront;
                } else if (event.key.code == sf::Keyboard::S) {
                    cameraPos -= cameraSpeed * cameraFront;
                } else if (event.key.code == sf::Keyboard::A) {
                    cameraPos -= normalize(cross(cameraFront, cameraUp)) * cameraSpeed;
                } else if (event.key.code == sf::Keyboard::D) {
                    cameraPos += normalize(cross(cameraFront, cameraUp)) * cameraSpeed;
                } else if (event.key.code == sf::Keyboard::Left) {
                    cameraFront = normalize(rotate(mat4(1.0f), rotationSpeed, cameraUp) * vec4(cameraFront, 0.0f));
                } else if (event.key.code == sf::Keyboard::Right) {
                    cameraFront = normalize(rotate(mat4(1.0f), -rotationSpeed, cameraUp) * vec4(cameraFront, 0.0f));
                } else if (event.key.code == sf::Keyboard::Up) {
                    vec3 right = normalize(cross(cameraFront, cameraUp));
                    cameraFront = normalize(rotate(mat4(1.0f), rotationSpeed, right) * vec4(cameraFront, 0.0f));
                } else if (event.key.code == sf::Keyboard::Down) {
                    vec3 right = normalize(cross(cameraFront, cameraUp));
                    cameraFront = normalize(rotate(mat4(1.0f), -rotationSpeed, right) * vec4(cameraFront, 0.0f));
                } else if (event.key.code == sf::Keyboard::Q) {
                    cameraPos += cameraSpeed * cameraUp;
                } else if (event.key.code == sf::Keyboard::E) {
                    cameraPos -= cameraSpeed * cameraUp;
                } else if (event.key.code == sf::Keyboard::Z) {
                    spheres[0].reflectivity = std::min(spheres[0].reflectivity + 0.1f, 1.0f);
                } else if (event.key.code == sf::Keyboard::X) {
                    spheres[0].reflectivity = std::max(spheres[0].reflectivity - 0.1f, 0.0f);
                } else if (event.key.code == sf::Keyboard::C) {
                    spheres[1].reflectivity = std::min(spheres[1].reflectivity + 0.1f, 1.0f);
                } else if (event.key.code == sf::Keyboard::V) {
                    spheres[1].reflectivity = std::max(spheres[1].reflectivity - 0.1f, 0.0f);
                } else if (event.key.code == sf::Keyboard::B) {
                    plane.reflectivity = std::min(plane.reflectivity + 0.1f, 1.0f);
                } else if (event.key.code == sf::Keyboard::N) {
                    plane.reflectivity = std::max(plane.reflectivity - 0.1f, 0.0f);
                }
                updateImage();
                texture.loadFromImage(image);
            }
        }

        window.clear();
        window.draw(sprite);
        window.display();
    }

    return 0;
}