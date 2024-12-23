#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>

struct Point {
    sf::CircleShape shape;
    sf::Vector2f position;

    Point(float x, float y) {
        position = {x, y};
        shape.setRadius(5);
        shape.setFillColor(sf::Color::Red);
        shape.setPosition(position);
    }

    void updatePosition(sf::Vector2f newPos) {
        position = newPos;
        shape.setPosition(position);
    }
};

sf::Vector2f bezierCurve(const std::vector<Point>& controlPoints, float t) {
    float u = 1 - t;
    return u * u * u * controlPoints[0].position +
           3 * u * u * t * controlPoints[1].position +
           3 * u * t * t * controlPoints[2].position +
           t * t * t * controlPoints[3].position;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "lab1");
    window.setFramerateLimit(60);

    std::vector<Point> controlPoints = {
        Point(100, 500),
        Point(300, 100),
        Point(500, 100),
        Point(700, 500)
    };

    std::vector<sf::Vertex> bezierCurveVertices;
    sf::Clock clock;
    bool isAnimating = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                isAnimating = !isAnimating;
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                for (auto& point : controlPoints) {
                    if (point.shape.getGlobalBounds().contains(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y)) {
                        point.shape.setFillColor(sf::Color::Green);
                    }
                }
            }
            if (event.type == sf::Event::MouseButtonReleased) {
                for (auto& point : controlPoints) {
                    point.shape.setFillColor(sf::Color::Red);
                }
            }
            if (event.type == sf::Event::MouseMoved) {
                for (auto& point : controlPoints) {
                    if (point.shape.getFillColor() == sf::Color::Green) {
                        point.updatePosition({static_cast<float>(sf::Mouse::getPosition(window).x - 5), static_cast<float>(sf::Mouse::getPosition(window).y - 5)});
                    }
                }
            }
        }

        if (isAnimating) {
            float time = clock.getElapsedTime().asSeconds();
            controlPoints[1].updatePosition({300 + std::sin(time) * 100, 100 + std::cos(time) * 50});
            controlPoints[2].updatePosition({500 + std::cos(time) * 100, 100 + std::sin(time) * 50});
        }

        bezierCurveVertices.clear();
        for (float t = 0; t <= 1; t += 0.01f) {
            sf::Vector2f bezierPoint = bezierCurve(controlPoints, t);
            bezierCurveVertices.emplace_back(bezierPoint, sf::Color::White);
        }

        window.clear();
        
        for (auto& point : controlPoints) {
            window.draw(point.shape);
        }

        window.draw(&bezierCurveVertices[0], bezierCurveVertices.size(), sf::PrimitiveType::LineStrip);
        
        window.display();
    }

    return 0;
}