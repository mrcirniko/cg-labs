#include <GL/glew.h> 
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// Структура вершины
struct Vertex {
    GLfloat position[3];
    GLfloat texCoord[2];
    GLfloat normal[3];
    GLfloat tangent[3];
    GLfloat bitangent[3];
};

// Вершины куба (полный массив вершины)
Vertex vertices[] = {
    // Позиции          // Текстурные координаты // Нормали         // Тангенты         // Битангенты
    // Передняя грань
    {{-1.0f, -1.0f,  1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    {{ 1.0f, -1.0f,  1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    {{ 1.0f,  1.0f,  1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    {{ 1.0f,  1.0f,  1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    {{-1.0f,  1.0f,  1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    {{-1.0f, -1.0f,  1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},

    // Задняя грань
    {{-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    {{-1.0f,  1.0f, -1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    {{ 1.0f,  1.0f, -1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    {{ 1.0f,  1.0f, -1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    {{ 1.0f, -1.0f, -1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    {{-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},

    // Левая грань
    {{-1.0f,  1.0f,  1.0f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}},
    {{-1.0f,  1.0f, -1.0f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}},
    {{-1.0f, -1.0f, -1.0f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}},
    {{-1.0f, -1.0f, -1.0f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}},
    {{-1.0f, -1.0f,  1.0f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}},
    {{-1.0f,  1.0f,  1.0f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}},

    // Правая грань
    {{ 1.0f,  1.0f,  1.0f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
    {{ 1.0f, -1.0f, -1.0f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
    {{ 1.0f,  1.0f, -1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
    {{ 1.0f, -1.0f, -1.0f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
    {{ 1.0f,  1.0f,  1.0f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
    {{ 1.0f, -1.0f,  1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},

    // Нижняя грань
    {{-1.0f, -1.0f, -1.0f}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
    {{ 1.0f, -1.0f, -1.0f}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
    {{ 1.0f, -1.0f,  1.0f}, {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
    {{ 1.0f, -1.0f,  1.0f}, {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
    {{-1.0f, -1.0f,  1.0f}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
    {{-1.0f, -1.0f, -1.0f}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},

    // Верхняя грань
    {{-1.0f,  1.0f, -1.0f}, {0.0f, 1.0f}, {0.0f,  1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    {{-1.0f,  1.0f,  1.0f}, {0.0f, 0.0f}, {0.0f,  1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    {{ 1.0f,  1.0f,  1.0f}, {1.0f, 0.0f}, {0.0f,  1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    {{ 1.0f,  1.0f,  1.0f}, {1.0f, 0.0f}, {0.0f,  1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    {{ 1.0f,  1.0f, -1.0f}, {1.0f, 1.0f}, {0.0f,  1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    {{-1.0f,  1.0f, -1.0f}, {0.0f, 1.0f}, {0.0f,  1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}}
};

// Функция для загрузки текстуры
GLuint loadTexture(const char* path) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    sf::Image imgData;
    if (!imgData.loadFromFile(path)) {
        std::cerr << "Failed to load texture" << std::endl;
        return 0;
    }
    // Генерация текстуры
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgData.getSize().x, imgData.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgData.getPixelsPtr());
    glGenerateMipmap(GL_TEXTURE_2D);
    return textureID;
}

// Вершинный шейдер
const char* vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;
out vec3 Tangent;
out vec3 Bitangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    Tangent = mat3(transpose(inverse(model))) * aTangent;
    Bitangent = mat3(transpose(inverse(model))) * aBitangent;
    TexCoord = aTexCoord;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";

// Фрагментный шейдер



const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;
in vec3 Tangent;
in vec3 Bitangent;

uniform sampler2D texture1;
uniform sampler2D normalMap;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform bool useNormalMap;

void main() {
    vec3 normal;
    if (useNormalMap) {
        normal = texture(normalMap, TexCoord).rgb;
        normal = normalize(normal * 2.0 - 1.0);
        vec3 T = normalize(Tangent);
        vec3 B = normalize(Bitangent);
        vec3 N = normalize(Normal);
        mat3 TBN = mat3(T, -B, N);
        normal = normalize(TBN * normal);
    } else {
        normal = normalize(Normal);
    }


    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0, 1.0, 1.0) * texture(texture1, TexCoord).rgb;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = vec3(0.5) * spec;

    vec3 result = diffuse + specular;
    FragColor = vec4(result, 1.0);
}
)";

// Компиляция шейдера
GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    // Проверка на ошибки компиляции
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    return shader;
}

// Создание шейдерной программы
GLuint createProgram(const char* vertexSource, const char* fragmentSource) {
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    // Проверка на ошибки компоновки
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

int main() {
    // Настройка окна
    sf::Window window(sf::VideoMode(800, 600), "lab4", sf::Style::Default, sf::ContextSettings(24));
    window.setVerticalSyncEnabled(true);

    // Включение буфера глубины
    glEnable(GL_DEPTH_TEST);

    // Инициализация GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Компиляция и создание шейдерной программы
    GLuint shaderProgram = createProgram(vertexShaderSource, fragmentShaderSource);

    // Настройка вершинного массива и буферов
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Настройка атрибутов вершин
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
    glEnableVertexAttribArray(4);

    // Загрузка текстур
    GLuint texture1 = loadTexture("2.jpg");
    GLuint normalMap = loadTexture("3.jpg");

    // Матрицы и углы поворота
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    float rotationX = 0.0f;
    float rotationY = 0.0f;

    // Основной цикл
    bool useNormalMap = true; // Флаг для переключения
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space) {
                    useNormalMap = !useNormalMap; // Переключение по пробелу
                } else if (event.key.code == sf::Keyboard::Up) {
                    rotationX -= 5.0f; // Поворот вверх
                } else if (event.key.code == sf::Keyboard::Down) {
                    rotationX += 5.0f; // Поворот вниз
                } else if (event.key.code == sf::Keyboard::Left) {
                    rotationY -= 5.0f; // Поворот влево
                } else if (event.key.code == sf::Keyboard::Right) {
                    rotationY += 5.0f; // Поворот вправо
                }
            }
        }

        // Очистка буфера цвета и глубины
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Использование шейдеров
        glUseProgram(shaderProgram);

        // Обновление модельной матрицы
        model = glm::rotate(glm::mat4(1.0f), glm::radians(rotationX), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f));

        // Установка униформов шейдеров
        GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
        GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
        GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        GLint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
        GLint viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
        GLint texture1Loc = glGetUniformLocation(shaderProgram, "texture1");
        GLint normalMapLoc = glGetUniformLocation(shaderProgram, "normalMap");
        GLint useNormalMapLoc = glGetUniformLocation(shaderProgram, "useNormalMap");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // Установка позиций света и камеры
        glUniform3f(lightPosLoc, 1.2f, 1.0f, 2.0f);
        glUniform3f(viewPosLoc, 0.0f, 0.0f, 3.0f);

        // Установка использования нормальной карты
        glUniform1i(useNormalMapLoc, useNormalMap);

        // Активизация текстур
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glUniform1i(texture1Loc, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalMap);
        glUniform1i(normalMapLoc, 1);

        // Отрисовка куба
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        window.display();
    }

    return 0;
}
