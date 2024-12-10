#pragma once

#include "UiElement.hpp"
#include <list>

class Layout : public UiElement
{
public:
    explicit Layout(Orientation orientation);

    Orientation orientation() const;
    void setOrientation(Orientation orientation);

    float spacing() const;
    void setSpacing(float spacing);

    float padding() const;
    void setPadding(float padding);

    void addEntry(std::unique_ptr<UiElement> &&entry);

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
    void show() override;
    void handleMousePressed(sf::Vector2i mousePosition) override;
    void handleMouseReleased(sf::Vector2i mousePosition) override;
    void handleMouseMoved(sf::Vector2i mousePosition) override;

private:
    void recalculateSizes();

    sf::Vector2f m_size{};
    float m_spacing{};
    float m_padding{};
    Orientation m_orientation{};
    std::list<std::unique_ptr<UiElement>> m_entries;
};