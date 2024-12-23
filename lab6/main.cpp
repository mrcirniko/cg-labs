#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <iomanip>
//g++ main.cpp -lGLEW -lGL -lGLU -lsfml-graphics -lsfml-window -lsfml-system -I/usr/include/glm

// Функция проверки столкновений камеры со сферой
bool checkSphereCollision(const glm::vec3 &cameraPos, const glm::vec3 &sphereCenter, float sphereRadius, float cameraHeight) {
    // Проверяем расстояние от центра сферы до позиции камеры с учетом роста
    float distance = glm::length(cameraPos - sphereCenter);
    return distance < (sphereRadius + cameraHeight);
}

// Функция проверки столкновений камеры с плоскостью
bool checkPlaneCollision(const glm::vec3 &cameraPos, const glm::vec3 &planePoint, const glm::vec3 &planeNormal, float cameraHeight) {
    return glm::dot(cameraPos - planePoint, planeNormal) < cameraHeight;
}

int main() {
    const int width = 1280;
    const int height = 1080;

    sf::RenderWindow window(sf::VideoMode(width, height), "lab6", sf::Style::Close);
    window.setVerticalSyncEnabled(true);
    window.setMouseCursorGrabbed(true);
    window.setMouseCursorVisible(false);

    sf::Shader shader;
    if (!shader.loadFromFile("shader.frag", sf::Shader::Fragment)) {
        std::cerr << "Failed to load shader" << std::endl;
        return -1;
    }

    float cameraHeight = 1.5f; // Высота камеры (рост персонажа)
    glm::vec3 cameraPos(0, cameraHeight, 5);
    glm::vec3 initialCameraPos = cameraPos; // Сохранение начальной позиции камеры
    glm::vec3 cameraFront(0, 0, -1);
    glm::vec3 cameraUp(0, 1, 0);
    float cameraSpeed = 0.05f; // Скорость движения камеры
    float angleY = 0.0f;
    float angleZ = 0.0f;

    glm::vec3 spheresCenter1(-1.5, 0, -5);
    glm::vec3 spheresCenter2(1.5, 0, -4);
    glm::vec3 spheresCenter3(-4.5, 0, -6);
    float spheresRadius1 = 1.0f;
    float spheresRadius2 = 1.0f;
    float spheresRadius3 = 1.0f;
    glm::vec3 spheresColor1(0, 1, 0);
    glm::vec3 spheresColor2(0, 0, 1);
    glm::vec3 spheresColor3(1, 0, 0);
    float spheresReflectivity1 = 0.5f;
    float spheresReflectivity2 = 0.8f;
    float spheresReflectivity3 = 0.3f;
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
    shader.setUniform("spheres[2].center", sf::Glsl::Vec3(spheresCenter3.x, spheresCenter3.y, spheresCenter3.z)); // Добавление зелёного шара
    shader.setUniform("spheres[2].radius", spheresRadius3);
    shader.setUniform("spheres[2].color", sf::Glsl::Vec3(spheresColor3.x, spheresColor3.y, spheresColor3.z));
    shader.setUniform("spheres[2].reflectivity", spheresReflectivity3);
    shader.setUniform("plane.point", sf::Glsl::Vec3(planePoint.x, planePoint.y, planePoint.z));
    shader.setUniform("plane.normal", sf::Glsl::Vec3(planeNormal.x, planeNormal.y, planeNormal.z));
    shader.setUniform("plane.color", sf::Glsl::Vec3(planeColor.x, planeColor.y, planeColor.z));
    shader.setUniform("plane.reflectivity", planeReflectivity);
    shader.setUniform("light.position", sf::Glsl::Vec3(2, 5, -3));
    shader.setUniform("light.color", sf::Glsl::Vec3(1, 1, 1));

    sf::RenderTexture renderTexture;
    if (!renderTexture.create(width, height)) {
        std::cerr << "Failed to create render texture" << std::endl;
        return -1;
    }

    sf::Sprite sprite(renderTexture.getTexture());

    sf::Mouse::setPosition(sf::Vector2i(window.getSize()) / 2, window);

    sf::Clock clock;
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Failed to load font" << std::endl;
        return -1;
    }

    sf::Text fpsText;
    fpsText.setFont(font);
    fpsText.setCharacterSize(24);
    fpsText.setFillColor(sf::Color::White);
    fpsText.setPosition(10.f, 10.f);

    sf::Text maxDepthText;
    maxDepthText.setFont(font);
    maxDepthText.setCharacterSize(24);
    maxDepthText.setFillColor(sf::Color::White);
    maxDepthText.setPosition(10.f, 40.f);

    // Переменные для гравитации и прыжка
    const float gravity = 0.005f;
    const float jumpSpeed = 0.1f;
    bool isJumping = false;
    float verticalSpeed = 0.0f;
    int maxDepth = 3;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space && !isJumping) {
                isJumping = true;
                verticalSpeed = jumpSpeed;
            }
        }

        glm::vec3 newCameraPos = cameraPos;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            newCameraPos.x += cameraSpeed * cameraFront.x;
            newCameraPos.z += cameraSpeed * cameraFront.z;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            newCameraPos.x -= cameraSpeed * cameraFront.x;
            newCameraPos.z -= cameraSpeed * cameraFront.z;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            glm::vec3 right = glm::normalize(glm::cross(cameraFront, cameraUp));
            newCameraPos.x -= right.x * cameraSpeed;
            newCameraPos.z -= right.z * cameraSpeed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            glm::vec3 right = glm::normalize(glm::cross(cameraFront, cameraUp));
            newCameraPos.x += right.x * cameraSpeed;
            newCameraPos.z += right.z * cameraSpeed;
        }

        // Применение гравитации
        if (isJumping) {
            newCameraPos.y += verticalSpeed;
            verticalSpeed -= gravity;
        }

        // Проверка столкновений камеры с сферами
        if (checkSphereCollision(newCameraPos, spheresCenter1, spheresRadius1, cameraHeight)) {
            glm::vec3 direction = glm::normalize(newCameraPos - spheresCenter1);
            newCameraPos = spheresCenter1 + direction * (spheresRadius1 + cameraHeight);
        }
        if (checkSphereCollision(newCameraPos, spheresCenter2, spheresRadius2, cameraHeight)) {
            glm::vec3 direction = glm::normalize(newCameraPos - spheresCenter2);
            newCameraPos = spheresCenter2 + direction * (spheresRadius2 + cameraHeight);
        }
        if (checkSphereCollision(newCameraPos, spheresCenter3, spheresRadius3, cameraHeight)) {
            newCameraPos = initialCameraPos;
        }

        // Проверка столкновений камеры с плоскостью
        if (checkPlaneCollision(newCameraPos, planePoint, planeNormal, cameraHeight)) {
            // Если камера ниже плоскости, то останавливаем падение и устанавливаем флаг isJumping в false
            newCameraPos.y = planePoint.y + cameraHeight;
            isJumping = false;
            verticalSpeed = 0.0f;
        }

        cameraPos = newCameraPos; // Обновляем позицию камеры

        // Обработка ввода для изменения отражаемости объектов
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
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::PageUp)) {
            maxDepth++;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::PageDown)) {
            if (maxDepth > 0) {
                maxDepth--;
            }
        }

        // Обработка ввода для вращения камеры
        sf::Vector2i mouseDelta = sf::Mouse::getPosition(window) - sf::Vector2i(window.getSize()) / 2;
        angleY += mouseDelta.x * 0.001f;
        angleZ -= mouseDelta.y * 0.001f;
        sf::Mouse::setPosition(sf::Vector2i(window.getSize()) / 2, window);

        glm::mat4 view = glm::lookAt(
            cameraPos,
            cameraPos + glm::vec3(cos(angleY), sin(angleZ), sin(angleY)),
            glm::vec3(0.f, 1.f, 0.f)
        );

        // Обновление направления камеры
        cameraFront = glm::normalize(glm::vec3(cos(angleY), sin(angleZ), sin(angleY)));

        shader.setUniform("cameraPos", sf::Glsl::Vec3(cameraPos.x, cameraPos.y, cameraPos.z));
        shader.setUniform("spheres[0].reflectivity", spheresReflectivity1);
        shader.setUniform("spheres[1].reflectivity", spheresReflectivity2);
        shader.setUniform("spheres[2].reflectivity", spheresReflectivity3);
        shader.setUniform("plane.reflectivity", planeReflectivity);
        shader.setUniform("view", sf::Glsl::Mat4(glm::value_ptr(view)));
        shader.setUniform("maxDepth", maxDepth);
        shader.setUniform("width", width);
        shader.setUniform("height", height);

        renderTexture.clear();
        renderTexture.draw(sprite, &shader);
        renderTexture.display();

        window.clear();
        window.draw(sprite);

        // Вычисление и отображение FPS
        sf::Time elapsed = clock.restart();
        float fps = 1.0f / elapsed.asSeconds();
        fpsText.setString("FPS: " + std::to_string(static_cast<int>(fps)));
        window.draw(fpsText);

        // Отображение значения maxDepth
        if (maxDepth > 0) {
            maxDepthText.setString("Max Depth of Ray Tracing: " + std::to_string(maxDepth));
        } else {
            maxDepthText.setString("Ray Tracing disabled");
        }
        window.draw(maxDepthText);

        window.display();
    }

    return 0;
}
