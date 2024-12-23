#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <GL/glu.h>
#include <cmath> // Для использования функций sin и cos

// Параметры камеры
float cameraX = 0.0f, cameraY = 0.0f, cameraZ = 5.0f;
float cameraAngleX = 0.0f, cameraAngleY = 0.0f;

// Текущий объект и его трансформации
int currentObject = 0;
constexpr int numObjects = 2;
struct Transform {
    float posX, posY, posZ;
    float scaleX, scaleY, scaleZ;
    float rotX, rotY, rotZ;
} transforms[numObjects] = {
    { -1.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f },
    {  1.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f }
};

void drawCube() {
    glBegin(GL_QUADS);
    // Front face
    glVertex3f(-1.0f, -1.0f,  1.0f);
    glVertex3f( 1.0f, -1.0f,  1.0f);
    glVertex3f( 1.0f,  1.0f,  1.0f);
    glVertex3f(-1.0f,  1.0f,  1.0f);
    // Back face
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f,  1.0f, -1.0f);
    glVertex3f( 1.0f,  1.0f, -1.0f);
    glVertex3f( 1.0f, -1.0f, -1.0f);
    // Top face
    glVertex3f(-1.0f,  1.0f, -1.0f);
    glVertex3f(-1.0f,  1.0f,  1.0f);
    glVertex3f( 1.0f,  1.0f,  1.0f);
    glVertex3f( 1.0f,  1.0f, -1.0f);
    // Bottom face
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f( 1.0f, -1.0f, -1.0f);
    glVertex3f( 1.0f, -1.0f,  1.0f);
    glVertex3f(-1.0f, -1.0f,  1.0f);
    // Right face
    glVertex3f( 1.0f, -1.0f, -1.0f);
    glVertex3f( 1.0f,  1.0f, -1.0f);
    glVertex3f( 1.0f,  1.0f,  1.0f);
    glVertex3f( 1.0f, -1.0f,  1.0f);
    // Left face
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f,  1.0f);
    glVertex3f(-1.0f,  1.0f,  1.0f);
    glVertex3f(-1.0f,  1.0f, -1.0f);
    glEnd();
}

void drawPyramid() {
    glBegin(GL_TRIANGLES);
    // Front face
    glVertex3f( 0.0f,  1.0f,  0.0f);
    glVertex3f(-1.0f, -1.0f,  1.0f);
    glVertex3f( 1.0f, -1.0f,  1.0f);
    // Right face
    glVertex3f( 0.0f,  1.0f,  0.0f);
    glVertex3f( 1.0f, -1.0f,  1.0f);
    glVertex3f( 1.0f, -1.0f, -1.0f);
    // Back face
    glVertex3f( 0.0f,  1.0f,  0.0f);
    glVertex3f( 1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    // Left face
    glVertex3f( 0.0f,  1.0f,  0.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f,  1.0f);
    glEnd();

    // Base of the pyramid
    glBegin(GL_QUADS);
    glVertex3f(-1.0f, -1.0f,  1.0f);
    glVertex3f( 1.0f, -1.0f,  1.0f);
    glVertex3f( 1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glEnd();
}

void applyTransform(const Transform& transform) {
    glTranslatef(transform.posX, transform.posY, transform.posZ);
    glRotatef(transform.rotX, 1.0f, 0.0f, 0.0f);
    glRotatef(transform.rotY, 0.0f, 1.0f, 0.0f);
    glRotatef(transform.rotZ, 0.0f, 0.0f, 1.0f);
    glScalef(transform.scaleX, transform.scaleY, transform.scaleZ);
}

int main() {
    sf::Window window(sf::VideoMode(1280, 1080), "lab3", sf::Style::Default, sf::ContextSettings(32));
    window.setVerticalSyncEnabled(true);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 4.0/3.0, 0.1, 100.0);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape)
                    window.close();
                // Управление камерой
                if (event.key.code == sf::Keyboard::W) cameraZ -= 0.1f;
                if (event.key.code == sf::Keyboard::S) cameraZ += 0.1f;
                if (event.key.code == sf::Keyboard::A) cameraX -= 0.1f;
                if (event.key.code == sf::Keyboard::D) cameraX += 0.1f;
                // Переключение между объектами
                if (event.key.code == sf::Keyboard::Tab) currentObject = (currentObject + 1) % numObjects;
                // Трансформации текущего объекта
                if (event.key.code == sf::Keyboard::Q) transforms[currentObject].posX -= 0.1f;
                if (event.key.code == sf::Keyboard::E) transforms[currentObject].posX += 0.1f;
                if (event.key.code == sf::Keyboard::R) transforms[currentObject].posY += 0.1f;
                if (event.key.code == sf::Keyboard::F) transforms[currentObject].posY -= 0.1f;
                if (event.key.code == sf::Keyboard::T) transforms[currentObject].posZ += 0.1f;
                if (event.key.code == sf::Keyboard::G) transforms[currentObject].posZ -= 0.1f;
                if (event.key.code == sf::Keyboard::Up) transforms[currentObject].rotX += 5.0f;
                if (event.key.code == sf::Keyboard::Down) transforms[currentObject].rotX -= 5.0f;
                if (event.key.code == sf::Keyboard::Left) transforms[currentObject].rotY += 5.0f;
                if (event.key.code == sf::Keyboard::Right) transforms[currentObject].rotY -= 5.0f;
                if (event.key.code == sf::Keyboard::O) transforms[currentObject].rotZ += 5.0f;
                if (event.key.code == sf::Keyboard::L) transforms[currentObject].rotZ -= 5.0f;
                if (event.key.code == sf::Keyboard::Y) transforms[currentObject].scaleX += 0.1f;
                if (event.key.code == sf::Keyboard::H) transforms[currentObject].scaleX -= 0.1f;
                if (event.key.code == sf::Keyboard::U) transforms[currentObject].scaleY += 0.1f;
                if (event.key.code == sf::Keyboard::J) transforms[currentObject].scaleY -= 0.1f;
                if (event.key.code == sf::Keyboard::I) transforms[currentObject].scaleZ += 0.1f;
                if (event.key.code == sf::Keyboard::K) transforms[currentObject].scaleZ -= 0.1f;
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(cameraX, cameraY, cameraZ, cameraX + sin(cameraAngleY), cameraY + sin(cameraAngleX), cameraZ - cos(cameraAngleY), 0.0f, 1.0f, 0.0f);

        for (int i = 0; i < numObjects; ++i) {
            glPushMatrix();
            applyTransform(transforms[i]);
            if (i == 0) {
                drawCube();
            } else if (i == 1) {
                drawPyramid();
            }
            glPopMatrix();
        }

        window.display();
    }

    return 0;
}