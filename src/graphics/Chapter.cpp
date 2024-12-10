#include "Chapter.hpp"

constexpr auto FullHeight = 7;
constexpr auto MinimizedHeight = 3;
const auto BackgroundColor = sf::Color{180, 180, 180, 100};
const auto FilledColor = sf::Color{255, 50, 50};

Chapter::Chapter(const FilmDetails::ChapterDetails &details)
    : m_details{details}
{
    m_backgroundShape.setFillColor(BackgroundColor);
    m_filledShape.setFillColor(FilledColor);
    m_label.setText(m_details.name);
}

FilmDetails::ChapterDetails Chapter::details() const
{
    return m_details;
}

float Chapter::filled() const
{
    return m_filledShape.getSize().x / size().x;
}

void Chapter::setFilled(float filled)
{
    m_filledShape.setSize({filled * size().x, getHeight()});
}

void Chapter::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    target.draw(m_backgroundShape, states);
    target.draw(m_filledShape, states);
    if (hovered()) {
        target.draw(m_label, states);
    }
}

void Chapter::updateGeometry()
{
    m_backgroundShape.setSize({size().x, getHeight()});
    m_filledShape.setSize({m_filledShape.getSize().x, getHeight()});
    m_label.setPosition(
        size().x / 2 - m_label.getGlobalBounds().width / 2,
        size().y / 2 - m_label.getGlobalBounds().height / 2 - FullHeight * 3);

    updateHeight();
}

void Chapter::onHoveredChanged()
{
    updateHeight();
}

float Chapter::getHeight() const
{
    return hovered() ? FullHeight : MinimizedHeight;
}

void Chapter::updateHeight()
{
    auto updateShape = [&](auto &shape) {
        shape.setSize({shape.getSize().x, getHeight()});
        shape.setPosition({shape.getPosition().x, (size().y - (hovered() ? FullHeight : MinimizedHeight)) / 2.f});
    };
    updateShape(m_backgroundShape);
    updateShape(m_filledShape);
}
