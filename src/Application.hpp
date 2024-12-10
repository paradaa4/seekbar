#pragma once

#include "FilmController.hpp"
#include "Layout.hpp"
#include <SFML/Graphics.hpp>

class Application
{
public:
    explicit Application(FilmController &controller);

    void run();

private:
    void setupUi();

    FilmController &m_filmController;
    sf::ContextSettings m_contextSettings;
    sf::RenderWindow m_window;
    Layout m_mainLayout{Orientation::Vertical};
};
