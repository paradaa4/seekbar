#include <SFML/Graphics.hpp>
#include <format>
#include <iostream>
#include <list>
#include <numeric>
#include <ranges>

enum class Orientation { Horizontal, Vertical };

class UiElement : public sf::Transformable, public sf::Drawable
{
public:
    virtual ~UiElement() = default;

    sf::Vector2f size() const { return m_size; }
    void setSize(sf::Vector2f size) { m_size = size; }
    float dimension(Orientation orientation) const
    {
        return orientation == Orientation::Horizontal ? size().x : size().y;
    }
    float secondaryDimension(Orientation orientation) const
    {
        return dimension(orientation == Orientation::Horizontal ? Orientation::Vertical : Orientation::Horizontal);
    }

    void setFillWidth(bool fillWidth) { m_fillWidth = fillWidth; };
    bool fillWidth() const { return m_fillWidth; }
    void setFillHeight(bool fillHeight) { m_fillHeight = fillHeight; };
    bool fillHeight() const { return m_fillHeight; }

    const std::unique_ptr<sf::Shape> &shape() const { return m_shape; }
    void setShape(std::unique_ptr<sf::Shape> &&shape)
    {
        m_shape = std::move(shape);
        if (m_shape) {
            m_size = m_shape->getGlobalBounds().getSize();
        }
    }

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override
    {
        if (m_shape) {
            m_shape->setPosition(getPosition());
            target.draw(*m_shape.get());
        }
    }

    void show() { updateGeometry(); }

private:
    virtual void updateGeometry(){};

    sf::Vector2f m_size{};
    bool m_fillWidth{};
    bool m_fillHeight{};
    std::unique_ptr<sf::Shape> m_shape;
};

// class Spacer : public UiElement
// {
// private:
//     void updateGeometry() {

//     }
// };

class VSpacer : public UiElement
{
public:
    explicit VSpacer() { setFillHeight(true); }
};

class HSpacer : public UiElement
{
public:
    explicit HSpacer() { setFillWidth(true); }
};
class PlayButton : public UiElement
{
public:
    explicit PlayButton()
    {
        auto shape = std::make_unique<sf::ConvexShape>();
        shape->setPointCount(3);
        shape->setPoint(0, {0, 0});
        shape->setPoint(1, {0, 20});
        shape->setPoint(2, {16, 10});
        setShape(std::move(shape));
    }
};

class SeekBar : public sf::RectangleShape
{
public:
    explicit SeekBar(const sf::Vector2f &size = {}) {}
};

class Layout : public UiElement
{
public:
    explicit Layout(Orientation orientation)
        : m_orientation{orientation}
    {
        setFillWidth(orientation == Orientation::Vertical);
        setFillWidth(orientation == Orientation::Horizontal);
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
            if (layout->orientation() == Orientation::Horizontal) {
                layout->setSize({size().x - 2 * m_padding, layout->size().y});
            } else {
                layout->setSize({layout->size().x, size().y - 2 * m_padding});
            }
        }
        m_entries.push_back(std::move(entry));
    }

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override
    {
        // sf::RectangleShape background{size()};
        // background.setPosition(getPosition());
        // background.setFillColor(sf::Color::Red - sf::Color{0, 0, 0, 150});
        // target.draw(background);
        for (const auto &entry : m_entries) {
            target.draw(*entry);
        }
    }

    void show()
    {
        updateMinimumSize();
        recalculateSizes();
        for (const auto &entry : m_entries) {
            entry->show();
        }
    }

private:
    void updateMinimumSize()
    {
        const auto minimumSize = 2 * m_padding
                                 + std::ranges::max(m_entries | std::views::transform([this](auto &entry) {
                                                        return entry->secondaryDimension(m_orientation);
                                                    }));

        if (m_orientation == Orientation::Horizontal) {
            setSize({size().x, minimumSize});
        } else {
            setSize({minimumSize, size().y});
        }
    }

    void recalculateSizes()
    {
        const auto remainingSize = dimension(m_orientation) - 2 * m_padding
                                   - std::accumulate(
                                       std::cbegin(m_entries),
                                       std::cend(m_entries),
                                       0.0f,
                                       [this](auto sum, const auto &entry) {
                                           return sum + entry->dimension(m_orientation);
                                       })
                                   - (m_entries.size() - 1) * m_spacing;

        auto spacers = m_entries | std::views::filter([this](auto &entry) {
                           return entry->fillWidth() && m_orientation == Orientation::Horizontal
                                  || entry->fillHeight() && m_orientation == Orientation::Vertical;
                       });

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
            std::cout << std::format("{}, {}\n", originPosition.x, originPosition.y);
            entry->setPosition(originPosition);
            const auto newPosition = m_spacing + entry->dimension(m_orientation);
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

    Layout layout{Orientation::Vertical};
    layout.setSize(sf::Vector2f{window.getSize()});
    layout.setSpacing(8);
    layout.setPadding(20);
    layout.addEntry(std::make_unique<VSpacer>());
    layout.addEntry(std::make_unique<PlayButton>());

    auto layout1 = std::make_unique<Layout>(Orientation::Horizontal);
    layout1->setSpacing(20);
    layout1->addEntry(std::make_unique<HSpacer>());
    layout1->addEntry(std::make_unique<PlayButton>());
    layout1->addEntry(std::make_unique<PlayButton>());
    layout1->addEntry(std::make_unique<HSpacer>());

    layout.addEntry(std::move(layout1));
    layout.addEntry(std::make_unique<VSpacer>());
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
