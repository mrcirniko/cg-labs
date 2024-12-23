#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <cmath>
#include <GL/glu.h>

// g++ -std=c++11 main.cpp -lsfml-window -lsfml-system -lGL -lGLU

void drawCube() {
    // Рисуем грани куба
    glBegin(GL_QUADS);

    // Передняя грань
    glColor3f(1.0f, 0.0f, 0.0f); // Красный
    glNormal3f(0.0f, 0.0f, 1.0f); // Нормаль
    glVertex3f(-1.0f, -1.0f,  1.0f);
    glVertex3f( 1.0f, -1.0f,  1.0f);
    glVertex3f( 1.0f,  1.0f,  1.0f);
    glVertex3f(-1.0f,  1.0f,  1.0f);

    // Задняя грань
    glColor3f(0.0f, 1.0f, 0.0f); // Зеленый
    glNormal3f(0.0f, 0.0f, -1.0f); // Нормаль
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f,  1.0f, -1.0f);
    glVertex3f( 1.0f,  1.0f, -1.0f);
    glVertex3f( 1.0f, -1.0f, -1.0f);

    // Левая грань
    glColor3f(0.0f, 0.0f, 1.0f); // Синий
    glNormal3f(-1.0f, 0.0f, 0.0f); // Нормаль
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f,  1.0f);
    glVertex3f(-1.0f,  1.0f,  1.0f);
    glVertex3f(-1.0f,  1.0f, -1.0f);

    // Правая грань
    glColor3f(1.0f, 1.0f, 0.0f); // Желтый
    glNormal3f(1.0f, 0.0f, 0.0f); // Нормаль
    glVertex3f( 1.0f, -1.0f, -1.0f);
    glVertex3f( 1.0f,  1.0f, -1.0f);
    glVertex3f( 1.0f,  1.0f,  1.0f);
    glVertex3f( 1.0f, -1.0f,  1.0f);

    // Верхняя грань
    glColor3f(1.0f, 0.5f, 0.5f); // Розовый
    glNormal3f(0.0f, 1.0f, 0.0f); // Нормаль
    glVertex3f(-1.0f,  1.0f, -1.0f);
    glVertex3f(-1.0f,  1.0f,  1.0f);
    glVertex3f( 1.0f,  1.0f,  1.0f);
    glVertex3f( 1.0f,  1.0f, -1.0f);

    // Нижняя грань
    glColor3f(0.5f, 0.5f, 0.5f); // Серый
    glNormal3f(0.0f, -1.0f, 0.0f); // Нормаль
    glVertex3f(-1.0f, -1.0f, -1.f);
    glVertex3f( 1.f, -1.f, -1.f);
    glVertex3f( 1.f, -1.f,  1.f);
    glVertex3f(-1.f, -1.f,  1.f);

    glEnd();

    // Рисуем линии для визуализации нормалей
    glLineWidth(2);
    glColor3f(1.f, 1.f, 1.f); // Белый

    glBegin(GL_LINES);

    // Нормали для граней куба
    float normal_length = 1.5f;

    // Передняя грань
    glVertex3f(0.f, 0.f, 1.f);
    glVertex3f(0.f, 0.f, 1.f + normal_length);

    // Задняя грань
    glVertex3f(0.f, 0.f, -1.f);
    glVertex3f(0.f, 0.f, -1.f - normal_length);

    // Левая грань
    glVertex3f(-1.f, 0.f, 0.f);
    glVertex3f(-1.f - normal_length, 0.f, 0.f);

    // Правая грань
    glVertex3f(1.f, 0.f, 0.f);
    glVertex3f(1.f + normal_length, 0.f, 0.f);

    // Верхняя грань
    glVertex3f(0.f, 1.f, 0.f);
    glVertex3f(0.f, 1.f + normal_length, 0.f);

    // Нижняя грань
    glVertex3f(0.f, -1.f, 0.f);
    glVertex3f(0.f, -1.f - normal_length, 0.f);

    glEnd();
}

int main() {
    sf::Window window(sf::VideoMode(800, 600), "lab2", sf::Style::Close | sf::Style::Titlebar);

    window.setActive();

    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    gluPerspective(45.f, (float)800 / (float)600, 0.01f, 100.f);
    glMatrixMode(GL_MODELVIEW);

    float angleX = 0.0f;
    float angleY = 0.0f;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Управление вращением куба
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) angleY -= 0.05f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) angleY += 0.05f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) angleX -= 0.05f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) angleX += 0.05f;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glLoadIdentity();
        gluLookAt(5.0f, 5.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

        // Вращение куба
        glRotatef(angleX, 1.0f, 0.0f, 0.0f);
        glRotatef(angleY, 0.0f, 1.0f, 0.0f);

        drawCube();

        window.display();
    }

    return EXIT_SUCCESS;
}