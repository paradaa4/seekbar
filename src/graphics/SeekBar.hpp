#pragma once

#include "Chapter.hpp"
#include "FilmController.hpp"
#include "UiElement.hpp"
#include <list>

class SeekBar : public UiElement
{
public:
    explicit SeekBar(FilmController &controller);

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
    void handleMouseMoved(sf::Vector2i mousePosition) override;

private:
    void updateGeometry() override;
    void onPressed(sf::Vector2i mousePosition) override;
    void onDragStarted() override;
    void onDragFinished() override;
    void onDragMove(sf::Vector2i mousePosition) override;

    void setCurrentTime(std::chrono::milliseconds currentTime);
    void updateChapters();

    FilmController &m_controller;
    std::chrono::milliseconds m_currentTime;
    std::list<std::unique_ptr<Chapter>> m_chapters;
    sf::CircleShape m_handle;
    bool m_wasPlaying{};
    int m_spacing{2};
};