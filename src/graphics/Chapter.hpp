#pragma once

#include "FilmDetails.hpp"
#include "Label.hpp"

class Chapter : public UiElement
{
public:
    explicit Chapter(const FilmDetails::ChapterDetails &details);

    FilmDetails::ChapterDetails details() const;

    float filled() const;
    void setFilled(float filled);

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

private:
    void updateGeometry() override;
    void onHoveredChanged() override;

    float getHeight() const;
    void updateHeight();

    FilmDetails::ChapterDetails m_details;
    sf::RectangleShape m_backgroundShape;
    sf::RectangleShape m_filledShape;
    Label m_label;
};
