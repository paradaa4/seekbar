#include "Application.hpp"
#include "CurrrentTimeLabel.hpp"
#include "PlayButton.hpp"
#include "SeekBar.hpp"
#include "Spacer.hpp"

Application::Application(FilmController &controller)
    : m_filmController{controller}
    , m_contextSettings{{}, {}, 8}
    , m_window{{600, 300}, "yt seeker", sf::Style::Resize | sf::Style::Close, m_contextSettings}
{
    m_window.setFramerateLimit(144);
    setupUi();
}

void Application::setupUi()
{
    m_mainLayout.setSize(sf::Vector2f{m_window.getSize()});
    m_mainLayout.setSpacing(4);
    m_mainLayout.setPadding(10);
    m_mainLayout.addEntry(std::make_unique<VSpacer>());
    m_mainLayout.addEntry(std::make_unique<SeekBar>(m_filmController));
    auto hLayout = std::make_unique<Layout>(Orientation::Horizontal);
    hLayout->setSize({0, 20});
    hLayout->addEntry(std::make_unique<PlayButton>(m_filmController));
    hLayout->addEntry(std::make_unique<HSpacer>(20));
    hLayout->addEntry(std::make_unique<CurrentTimeLabel>(m_filmController));
    hLayout->addEntry(std::make_unique<HSpacer>());
    m_mainLayout.addEntry(std::move(hLayout));
    m_mainLayout.show();
}

void Application::run()
{
    sf::Clock loadingClock;
    while (m_window.isOpen()) {
        for (auto event = sf::Event(); m_window.pollEvent(event);) {
            const auto mousePosition = sf::Mouse::getPosition(m_window);
            if (event.type == sf::Event::Closed) {
                m_window.close();
            } else if (event.type == sf::Event::MouseMoved) {
                m_mainLayout.handleMouseMoved(mousePosition);
            } else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                m_mainLayout.handleMousePressed(mousePosition);
            } else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                m_mainLayout.handleMouseReleased(mousePosition);
            } else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Left) {
                    m_filmController.jumpBackwards();
                } else if (event.key.code == sf::Keyboard::Right) {
                    m_filmController.jumpForward();
                } else if (event.key.code == sf::Keyboard::Space && !m_filmController.loading()) {
                    if (m_filmController.playing()) {
                        m_filmController.pause();
                    } else {
                        m_filmController.play();
                    }
                }
            }
        }
        if (loadingClock.getElapsedTime() > sf::seconds(3) && m_filmController.loading()) {
            m_filmController.pause();
        }
        m_filmController.update();
        m_window.clear(sf::Color{37, 38, 40});
        m_window.draw(m_mainLayout);
        m_window.display();
    }
}