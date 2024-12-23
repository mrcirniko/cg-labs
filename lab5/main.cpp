#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

int main() {
    const int width = 800;
    const int height = 600;

    sf::RenderWindow window(sf::VideoMode(width, height), "Ray Tracing with GLSL", sf::Style::Close);
    window.setVerticalSyncEnabled(true);
    window.setMouseCursorGrabbed(true);
    window.setMouseCursorVisible(false);

    sf::Shader shader;
    if (!shader.loadFromFile("shader.frag", sf::Shader::Fragment)) {
        std::cerr << "Failed to load shader" << std::endl;
        return -1;
    }

    glm::vec3 cameraPos(0, 2, 5);
    glm::vec3 cameraFront(0, 0, -1);
    glm::vec3 cameraUp(0, 1, 0);
    float cameraSpeed = 0.05f; // Скорость движения камеры

    glm::vec3 spheresCenter1(-1.5, 0, -5);
    glm::vec3 spheresCenter2(1.5, 0, -4);
    float spheresRadius1 = 1.0f;
    float spheresRadius2 = 1.0f;
    glm::vec3 spheresColor1(1, 0, 0);
    glm::vec3 spheresColor2(0, 0, 1);
    float spheresReflectivity1 = 0.5f;
    float spheresReflectivity2 = 0.8f;
    glm::vec3 planePoint(0, -1, 0);
    glm::vec3 planeNormal(0, 1, 0);
    glm::vec3 planeColor(0.5, 0.5, 0.5);
    float planeReflectivity = 0.3f;

    shader.setUniform("cameraPos", sf::Glsl::Vec3(cameraPos.x, cameraPos.y, cameraPos.z));
    shader.setUniform("spheres[0].center", sf::Glsl::Vec3(spheresCenter1.x, spheresCenter1.y, spheresCenter1.z));
    shader.setUniform("spheres[0].radius", spheresRadius1);
    shader.setUniform("spheres[0].color", sf::Glsl::Vec3(spheresColor1.x, spheresColor1.y, spheresColor1.z));
    shader.setUniform("spheres[0].reflectivity", spheresReflectivity1);
    shader.setUniform("spheres[1].center", sf::Glsl::Vec3(spheresCenter2.x, spheresCenter2.y, spheresCenter2.z));
    shader.setUniform("spheres[1].radius", spheresRadius2);
    shader.setUniform("spheres[1].color", sf::Glsl::Vec3(spheresColor2.x, spheresColor2.y, spheresColor2.z));
    shader.setUniform("spheres[1].reflectivity", spheresReflectivity2);
    shader.setUniform("plane.point", sf::Glsl::Vec3(planePoint.x, planePoint.y, planePoint.z));
    shader.setUniform("plane.normal", sf::Glsl::Vec3(planeNormal.x, planeNormal.y, planeNormal.z));
    shader.setUniform("plane.color", sf::Glsl::Vec3(planeColor.x, planeColor.y, planeColor.z));
    shader.setUniform("plane.reflectivity", planeReflectivity);
    shader.setUniform("light.position", sf::Glsl::Vec3(2, 5, -3));
    shader.setUniform("light.color", sf::Glsl::Vec3(1, 1, 1)); // Убедитесь, что эта переменная используется

    sf::RenderTexture renderTexture;
    if (!renderTexture.create(width, height)) {
        std::cerr << "Failed to create render texture" << std::endl;
        return -1;
    }

    sf::Sprite sprite(renderTexture.getTexture());

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            cameraPos += cameraSpeed * cameraFront;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            cameraPos -= cameraSpeed * cameraFront;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
            spheresReflectivity1 = std::min(spheresReflectivity1 + 0.01f, 1.0f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            spheresReflectivity1 = std::max(spheresReflectivity1 - 0.01f, 0.0f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
            spheresReflectivity2 = std::min(spheresReflectivity2 + 0.01f, 1.0f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
            spheresReflectivity2 = std::max(spheresReflectivity2 - 0.01f, 0.0f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::U)) {
            planeReflectivity = std::min(planeReflectivity + 0.01f, 1.0f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::I)) {
            planeReflectivity = std::max(planeReflectivity - 0.01f, 0.0f);
        }



        shader.setUniform("cameraPos", sf::Glsl::Vec3(cameraPos.x, cameraPos.y, cameraPos.z));
        shader.setUniform("spheres[0].reflectivity", spheresReflectivity1);
        shader.setUniform("spheres[1].reflectivity", spheresReflectivity2);
        shader.setUniform("plane.reflectivity", planeReflectivity);

        renderTexture.clear();
        renderTexture.draw(sprite, &shader);
        renderTexture.display();

        window.clear();
        window.draw(sprite);
        window.display();
    }

    return 0;
}