#include <SFML/Graphics.hpp>
#include <format>
#include <iostream>
#include <list>
#include <numeric>
#include <ranges>

using UiElement = sf::Shape;
// class UiElement : public sf::Shape {};

class Spacer : public sf::RectangleShape
{
public:
    explicit Spacer(const sf::Vector2f &size = {}) { setFillColor(sf::Color::Cyan); }
};

class PlayButton : public sf::ConvexShape
{
public:
    explicit PlayButton(std::size_t pointCount = 0)
    {
        setPointCount(3);
        setPoint(0, {0, 0});
        setPoint(1, {0, 20});
        setPoint(2, {16, 10});
    }
};

class Layout : public sf::RectangleShape
{
public:
    enum class Orientation { Horizontal, Vertical };

    explicit Layout(sf::Vector2f size, Orientation orientation)
        : m_orientation{orientation}
    {
        setSize(size);
    }

    Orientation orientation() const { return m_orientation; }
    void setOrientation(Orientation orientation) { m_orientation = orientation; }

    float spacing() const { return m_spacing; }
    void setSpacing(float spacing) { m_spacing = spacing; }

    float padding() const { return m_padding; }
    void setPadding(float padding) { m_padding = padding; }

    void addEntry(std::unique_ptr<UiElement> &&entry)
    {
        if (const auto layout = dynamic_cast<Layout *>(entry.get())) {
            if (layout->orientation() == Layout::Orientation::Horizontal) {
                layout->setSize({getSize().x - 2 * m_padding, layout->getSize().y});
            } else {
                layout->setSize({layout->getSize().x, getSize().y - 2 * m_padding});
            }
        }
        m_entries.push_back(std::move(entry));
    }

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override
    {
        // sf::RectangleShape background{getSize()};
        // background.setPosition(getPosition());
        // background.setFillColor(getFillColor() - sf::Color{0, 0, 0, 150});
        // target.draw(background);
        for (const auto &entry : m_entries) {
            target.draw(*entry);
        }
    }

    void show()
    {
        updateMinimumSize();
        recalculateSizes();
    }

private:
    float entrySize(const std::unique_ptr<UiElement> &element)
    {
        return m_orientation == Orientation::Horizontal ? element->getGlobalBounds().width
                                                        : element->getGlobalBounds().height;
    }

    void updateMinimumSize()
    {
        const auto minimumSize = 2 * m_padding
                                 + std::ranges::max(m_entries | std::views::transform([this](auto &entry) {
                                                        return m_orientation == Orientation::Horizontal
                                                                   ? entry->getGlobalBounds().height
                                                                   : entry->getGlobalBounds().width;
                                                    }));

        std::cout << minimumSize << std::endl;
        if (m_orientation == Orientation::Horizontal) {
            setSize({getSize().x, minimumSize});
        } else {
            setSize({minimumSize, getSize().y});
        }
    }

    void recalculateSizes()
    {
        const auto remainingSize = (m_orientation == Orientation::Horizontal ? getSize().x : getSize().y)
                                   - 2 * m_padding
                                   - std::accumulate(
                                       std::cbegin(m_entries),
                                       std::cend(m_entries),
                                       0.0f,
                                       [this](auto sum, const auto &entry) { return sum + entrySize(entry); })
                                   - (m_entries.size() - 1) * m_spacing;

        auto spacers = m_entries
                       | std::views::transform([](auto &entry) { return dynamic_cast<Spacer *>(entry.get()); })
                       | std::views::filter([](auto spacer) { return spacer != nullptr; });

        const auto sizePerSpacer = remainingSize / std::ranges::distance(spacers);
        for (const auto &entry : spacers) {
            if (m_orientation == Orientation::Horizontal) {
                entry->setSize(sf::Vector2f{sizePerSpacer, 0});
            } else {
                entry->setSize(sf::Vector2f{0, sizePerSpacer});
            }
        }
        auto originPosition = this->getPosition() + sf::Vector2f{m_padding, m_padding};
        for (const auto &entry : m_entries) {
            entry->setPosition(originPosition);
            const auto newPosition = m_spacing + entrySize(entry);
            if (m_orientation == Orientation::Horizontal) {
                originPosition.x += newPosition;
            } else {
                originPosition.y += newPosition;
            };
        }

        for (auto layout : m_entries | std::views::transform([](auto &entry) {
                               return dynamic_cast<Layout *>(entry.get());
                           }) | std::views::filter([](auto layout) { return layout != nullptr; })) {
            layout->show();
        }
    }

    sf::Vector2f m_size{};
    float m_spacing{};
    float m_padding{};
    Orientation m_orientation{};
    std::list<std::unique_ptr<UiElement>> m_entries;
};

int main()
{
    auto window = sf::RenderWindow({800u, 600u}, "yt seeker");
    window.setFramerateLimit(144);

    Layout layout{sf::Vector2f{window.getSize()}, Layout::Orientation::Vertical};
    layout.setFillColor(sf::Color::Yellow);
    layout.setSpacing(8);
    layout.setPadding(10);
    layout.addEntry(std::make_unique<Spacer>());
    layout.addEntry(std::make_unique<PlayButton>());

    auto layout1 = std::make_unique<Layout>(sf::Vector2f{0, 20}, Layout::Orientation::Horizontal);
    layout1->setSpacing(20);
    layout1->setFillColor(sf::Color::Red);
    layout1->addEntry(std::make_unique<Spacer>());
    layout1->addEntry(std::make_unique<PlayButton>());
    layout1->addEntry(std::make_unique<PlayButton>());
    layout1->addEntry(std::make_unique<Spacer>());

    layout.addEntry(std::move(layout1));
    layout.addEntry(std::make_unique<Spacer>());
    layout.show();

    while (window.isOpen()) {
        for (auto event = sf::Event(); window.pollEvent(event);) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        window.clear(sf::Color{37, 38, 40});
        window.draw(layout);

        window.display();
    }
}
