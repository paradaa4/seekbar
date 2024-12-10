#include "Layout.hpp"
#include <numeric>
#include <ranges>

Layout::Layout(Orientation orientation)
    : m_orientation{orientation}
{
    setFillWidth(orientation == Orientation::Vertical);
    setFillWidth(orientation == Orientation::Horizontal);
}

Orientation Layout::orientation() const
{
    return m_orientation;
}

void Layout::setOrientation(Orientation orientation)
{
    m_orientation = orientation;
}

float Layout::spacing() const
{
    return m_spacing;
}

void Layout::setSpacing(float spacing)
{
    m_spacing = spacing;
}
float Layout::padding() const
{
    return m_padding;
}

void Layout::setPadding(float padding)
{
    m_padding = padding;
}

void Layout::addEntry(std::unique_ptr<UiElement> &&entry)
{
    m_entries.push_back(std::move(entry));
}

void Layout::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    for (const auto &entry : m_entries) {
        target.draw(*entry);
    }
    UiElement::draw(target, states);
}

void Layout::show()
{
    recalculateSizes();
    for (const auto &entry : m_entries) {
        entry->show();
    }
}

void Layout::handleMousePressed(sf::Vector2i mousePosition)
{
    for (const auto &entry : m_entries) {
        entry->handleMousePressed(mousePosition);
    }
}

void Layout::handleMouseReleased(sf::Vector2i mousePosition)
{
    for (const auto &entry : m_entries) {
        entry->handleMouseReleased(mousePosition);
    }
}

void Layout::handleMouseMoved(sf::Vector2i mousePosition)
{
    mousePosition -= sf::Vector2i{getPosition()};
    for (const auto &entry : m_entries) {
        entry->handleMouseMoved(mousePosition);
    }
}

void Layout::recalculateSizes()
{
    const auto remainingSize = dimension(m_orientation) - 2 * m_padding
                               - std::accumulate(
                                   std::cbegin(m_entries),
                                   std::cend(m_entries),
                                   0.0f,
                                   [this](auto sum, const auto &entry) { return sum + entry->dimension(m_orientation); })
                               - (m_entries.size() - 1) * m_spacing;

    const auto sizePerSpacer = remainingSize / std::ranges::count_if(m_entries, [this](const auto &entry) {
                                   return entry->fillWidth() && m_orientation == Orientation::Horizontal
                                          || entry->fillHeight() && m_orientation == Orientation::Vertical;
                               });
    for (const auto &entry : m_entries) {
        if (entry->fillWidth()) {
            entry->setSize(
                {m_orientation == Orientation::Horizontal ? sizePerSpacer : size().x - 2 * m_padding, entry->size().y});
        } else if (entry->fillHeight()) {
            entry->setSize(
                {entry->size().x, m_orientation == Orientation::Vertical ? sizePerSpacer : size().y - 2 * m_padding});
        }
    }
    auto originPosition = this->getPosition() + sf::Vector2f{m_padding, m_padding};
    for (const auto &entry : m_entries) {
        entry->setPosition(originPosition);
        const auto newPosition = m_spacing + entry->dimension(m_orientation);
        if (m_orientation == Orientation::Horizontal) {
            originPosition.x += newPosition;
        } else {
            originPosition.y += newPosition;
        };
    }
}
