#include "CurrrentTimeLabel.hpp"
#include "FilmController.hpp"
#include "Layout.hpp"
#include "PlayButton.hpp"
#include "SeekBar.hpp"
#include "Spacer.hpp"
#include <SFML/Graphics.hpp>
#include <chrono>
#include <format>
#include <functional>
#include <iostream>
#include <list>
#include <numbers>
#include <numeric>
#include <random>
#include <ranges>

int main()
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8.0;
    auto window = sf::RenderWindow({600, 300}, "yt seeker", sf::Style::Resize | sf::Style::Close, settings);
    window.setFramerateLimit(144);

    FilmController filmController;
    filmController.setFilmDetails(
        {.name = "test",
         .duration = std::chrono::seconds{100},
         .chapters
         = {{.name = "Intro", .startTime = std::chrono::seconds{0}, .endTime = std::chrono::seconds{10}},
            {.name = "Explanation", .startTime = std::chrono::seconds{10}, .endTime = std::chrono::seconds{70}},
            {.name = "Summary", .startTime = std::chrono::seconds{70}, .endTime = std::chrono::seconds{85}},
            {.name = "Goodbye", .startTime = std::chrono::seconds{85}, .endTime = std::chrono::seconds{100}}}});

    Layout layout{Orientation::Vertical};
    layout.setSize(sf::Vector2f{window.getSize()});
    layout.setSpacing(4);
    layout.setPadding(10);
    layout.addEntry(std::make_unique<VSpacer>());
    layout.addEntry(std::make_unique<SeekBar>(filmController));
    auto hLayout = std::make_unique<Layout>(Orientation::Horizontal);
    hLayout->setSize({0, 20});
    hLayout->addEntry(std::make_unique<PlayButton>(filmController));
    hLayout->addEntry(std::make_unique<HSpacer>(20));
    hLayout->addEntry(std::make_unique<CurrentTimeLabel>(filmController));
    hLayout->addEntry(std::make_unique<HSpacer>());
    layout.addEntry(std::move(hLayout));
    layout.show();

    sf::Clock loadingClock;
    while (window.isOpen()) {
        for (auto event = sf::Event(); window.pollEvent(event);) {
            const auto mousePosition = sf::Mouse::getPosition(window);
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::MouseMoved) {
                layout.handleMouseMoved(mousePosition);
            } else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                layout.handleMousePressed(mousePosition);
            } else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                layout.handleMouseReleased(mousePosition);
            } else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Left) {
                    filmController.jumpBackwards();
                } else if (event.key.code == sf::Keyboard::Right) {
                    filmController.jumpForward();
                } else if (event.key.code == sf::Keyboard::Space && !filmController.loading()) {
                    if (filmController.playing()) {
                        filmController.pause();
                    } else {
                        filmController.play();
                    }
                }
            }
        }
        if (loadingClock.getElapsedTime() > sf::seconds(3) && filmController.loading()) {
            filmController.pause();
        }
        filmController.update();
        window.clear(sf::Color{37, 38, 40});
        window.draw(layout);
        window.display();
    }
}
