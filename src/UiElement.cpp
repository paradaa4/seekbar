#include "UiElement.hpp"

sf::Vector2f UiElement::size() const
{
    return m_size;
}

void UiElement::setSize(sf::Vector2f size)
{
    m_size = size;
    updateGeometry();
}

float UiElement::dimension(Orientation orientation) const
{
    return orientation == Orientation::Horizontal ? size().x : size().y;
}

float UiElement::secondaryDimension(Orientation orientation) const
{
    return dimension(orientation == Orientation::Horizontal ? Orientation::Vertical : Orientation::Horizontal);
}

void UiElement::setFillWidth(bool fillWidth)
{
    m_fillWidth = fillWidth;
}

bool UiElement::fillWidth() const
{
    return m_fillWidth;
}

void UiElement::setFillHeight(bool fillHeight)
{
    m_fillHeight = fillHeight;
}

bool UiElement::fillHeight() const
{
    return m_fillHeight;
}

const std::unique_ptr<sf::Shape> &UiElement::shape() const
{
    return m_shape;
}

void UiElement::setShape(std::unique_ptr<sf::Shape> &&shape)
{
    m_shape = std::move(shape);
    if (m_shape) {
        m_size = m_shape->getGlobalBounds().getSize();
    }
}

void UiElement::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    if (m_shape) {
        target.draw(*m_shape.get(), states);
    }
}

void UiElement::show()
{
    updateGeometry();
}

void UiElement::handleMousePressed(sf::Vector2i mousePosition)
{
    if (containsMouse(mousePosition)) {
        m_pressed = true;
        onPressed(mousePosition);
    }
}

void UiElement::handleMouseReleased(sf::Vector2i mousePosition)
{
    if (m_pressed) {
        m_pressed = false;
        onReleased();
        if (m_dragged) {
            m_dragged = false;
            onDragFinished();
        }
    }
}

void UiElement::handleMouseMoved(sf::Vector2i mousePosition)
{
    if (const auto hovered = containsMouse(mousePosition); hovered != m_hovered) {
        m_hovered = hovered;
        onHoveredChanged();
    }
    if (m_pressed) {
        if (!m_dragged) {
            m_dragged = true;
            onDragStarted();
        }
        onDragMove(mousePosition);
    }
}

bool UiElement::pressed() const
{
    return m_pressed;
}

bool UiElement::hovered() const
{
    return m_hovered;
}

bool UiElement::dragged() const
{
    return m_dragged;
}

sf::FloatRect UiElement::rect() const
{
    return {getPosition(), m_size};
}

bool UiElement::containsMouse(sf::Vector2i mousePosition) const
{
    return rect().contains(sf::Vector2f{mousePosition});
}
