#include <SFML/Graphics.hpp>

int main() {
  auto window = sf::RenderWindow({800, 600u}, "yt seeker");
  window.setFramerateLimit(144);

  while (window.isOpen()) {
    for (auto event = sf::Event(); window.pollEvent(event);) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
    }

    window.clear();

    sf::CircleShape circle;
    circle.setRadius(100);
    circle.setFillColor(sf::Color::Red);
    circle.setPosition(100, 100);
    window.draw(circle);

    window.display();
  }
}
