#pragma once

#include "FilmController.hpp"
#include "UiElement.hpp"

class PlayButton : public UiElement
{
public:
    explicit PlayButton(FilmController &controller);

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

private:
    void onPressed(sf::Vector2i mousePosition) override;

    FilmController &m_controller;
    std::vector<std::unique_ptr<sf::RectangleShape>> m_pausedShapes;
    std::vector<std::unique_ptr<sf::CircleShape>> m_loadingShapes;
    std::unique_ptr<sf::ConvexShape> m_playShape;
    std::unique_ptr<sf::RectangleShape> m_restartShape;
    sf::Clock m_clock;
};
