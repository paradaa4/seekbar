#pragma once

#include "Types.hpp"
#include <SFML/Graphics.hpp>
#include <memory>

class UiElement : public sf::Transformable, public sf::Drawable
{
public:
    virtual ~UiElement() = default;

    sf::Vector2f size() const;
    void setSize(sf::Vector2f size);

    const std::unique_ptr<sf::Shape> &shape() const;
    void setShape(std::unique_ptr<sf::Shape> &&shape);

    float dimension(Orientation orientation) const;
    float secondaryDimension(Orientation orientation) const;

    bool fillWidth() const;
    void setFillWidth(bool fillWidth);

    bool fillHeight() const;
    void setFillHeight(bool fillHeight);

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

    virtual void show();

    virtual void handleMousePressed(sf::Vector2i mousePosition);
    virtual void handleMouseReleased(sf::Vector2i mousePosition);
    virtual void handleMouseMoved(sf::Vector2i mousePosition);

    bool pressed() const;
    bool hovered() const;
    bool dragged() const;

protected:
    sf::FloatRect rect() const;
    bool containsMouse(sf::Vector2i mousePosition) const;

private:
    virtual void updateGeometry(){};
    virtual void onPressed(sf::Vector2i mousePosition) {}
    virtual void onReleased() {}
    virtual void onHoveredChanged() {}
    virtual void onDragStarted() {}
    virtual void onDragFinished() {}
    virtual void onDragMove(sf::Vector2i mousePosition) {}

    sf::Vector2f m_size{};
    std::unique_ptr<sf::Shape> m_shape;
    bool m_fillWidth{};
    bool m_fillHeight{};
    bool m_pressed{};
    bool m_hovered{};
    bool m_dragged{};
};
