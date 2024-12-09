#include <SFML/Graphics.hpp>
#include <chrono>
#include <format>
#include <functional>
#include <iostream>
#include <list>
#include <numeric>
#include <random>
#include <ranges>

auto format(auto v)
{
    return std::format("({}, {})\n", v.x, v.y);
}

enum class Orientation { Horizontal, Vertical };

class FilmTimer
{
public:
    using Callback = std::function<void(std::chrono::milliseconds)>;

    std::chrono::milliseconds currentTime() const { return m_currentTime; }
    bool playing() const { return m_playing; }
    void play()
    {
        m_playing = true;
        m_clock.restart();
    }
    void stop() { m_playing = false; }
    void jumpBackwards() { jump(-seekInterval); }
    void jumpForward() { jump(seekInterval); }
    void jumpTo(auto time) { jump(time - m_currentTime); }
    void update()
    {
        if (m_playing) {
            m_currentTime += std::chrono::milliseconds{m_clock.getElapsedTime().asMilliseconds()};
            m_clock.restart();
            m_callback(m_currentTime);
        }
    }

    void onCurrnetTimeChanged(Callback &&callback) { m_callback = std::move(callback); }

private:
    static constexpr auto seekInterval = std::chrono::seconds{10};

    void jump(auto interval)
    {
        m_currentTime += interval;
        m_callback(m_currentTime);
    }

    Callback m_callback;
    bool m_playing{};
    sf::Clock m_clock;
    std::chrono::milliseconds m_currentTime{};
};

class UiElement : public sf::Transformable, public sf::Drawable
{
public:
    virtual ~UiElement() = default;

    sf::Vector2f size() const { return m_size; }
    void setSize(sf::Vector2f size)
    {
        m_size = size;
        updateGeometry();
    }

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
        // sf::RectangleShape background{size()};
        // background.setPosition(getPosition());
        // background.setFillColor(m_debugBackgroundColor);
        // target.draw(background);
        states.transform *= getTransform();
        if (m_shape) {
            target.draw(*m_shape.get(), states);
        }
    }

    virtual void show() { updateGeometry(); }
    virtual void onClicked(sf::Vector2i mousePosition) {}
    virtual void onMouseMoved(sf::Vector2i mousePosition) {}

protected:
    sf::Rect<float> rect() const { return {getPosition(), m_size}; }
    bool containsMouse(sf::Vector2i mousePosition) const
    {
        return sf::Rect<float>{getPosition(), m_size}.contains(sf::Vector2f{mousePosition});
    }

private:
    virtual void updateGeometry(){};

    sf::Vector2f m_size{};
    bool m_fillWidth{};
    bool m_fillHeight{};
    std::unique_ptr<sf::Shape> m_shape;

    sf::Color m_debugBackgroundColor = []() {
        static std::random_device rd;
        static std::mt19937 mt{rd()};
        static std::uniform_int_distribution<sf::Uint8> dist{0, 255};
        return sf::Color{dist(mt), dist(mt), dist(mt), 90};
    }();
};

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
    using Callback = std::function<void(bool)>;
    explicit PlayButton(Callback &&callback)
        : m_callback{std::move(callback)}
    {
        static constexpr auto padding = 2.f;

        setSize({20, 20});
        m_playShape = std::make_unique<sf::ConvexShape>();
        m_playShape->setPointCount(3);
        m_playShape->setPoint(0, {padding, padding});
        m_playShape->setPoint(1, {padding, size().y - padding});
        m_playShape->setPoint(2, {size().x - padding * 2, size().y / 2});
        auto createPauseShape = [this](int offset = 0) {
            auto shape = std::make_unique<sf::RectangleShape>();
            shape->setPosition({padding + offset, padding});
            shape->setSize({2 * padding, size().y - 2 * padding});
            return shape;
        };

        m_pauseShapes.push_back(createPauseShape());
        m_pauseShapes.push_back(createPauseShape(10));
    }

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override
    {
        states.transform *= getTransform();
        if (m_playing) {
            for (const auto &shape : m_pauseShapes) {
                target.draw(*shape.get(), states);
            }
        } else {
            target.draw(*m_playShape.get(), states);
        }
    }

    void onClicked(sf::Vector2i mousePosition) override
    {
        if (containsMouse(mousePosition)) {
            m_playing = !m_playing;
        }
        m_callback(m_playing);
    }

private:
    Callback m_callback;
    bool m_playing{};
    std::vector<std::unique_ptr<sf::RectangleShape>> m_pauseShapes;
    std::unique_ptr<sf::ConvexShape> m_playShape;
};

struct ChapterDetails
{
    std::string name;
    std::chrono::milliseconds startTime{};
    std::chrono::milliseconds endTime{};

    std::chrono::milliseconds duration() { return endTime - startTime; }
};

struct FilmDetails
{
    std::string name;
    std::chrono::milliseconds duration{};
    std::vector<ChapterDetails> chapters;
};

class Chapter : public UiElement
{
public:
    explicit Chapter(const ChapterDetails &details)
        : m_details{details}
    {
        setSize({0, fullHeight});
        m_backgroundShape.setFillColor(sf::Color{180, 180, 180, 100});
        m_filledShape.setFillColor(sf::Color::Red);
    }

    ChapterDetails details() const { return m_details; }

    void setFilled(float filled) { m_filledShape.setSize({filled * size().x, getHeight()}); }

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override
    {
        states.transform *= getTransform();
        target.draw(m_backgroundShape, states);
        target.draw(m_filledShape, states);
    }

    std::optional<std::chrono::milliseconds> tryJump(sf::Vector2i mousePosition)
    {
        if (containsMouse(mousePosition)) {
            auto progress = (mousePosition.x - getPosition().x) / size().x;
            return std::chrono::duration_cast<std::chrono::milliseconds>(
                m_details.startTime + (m_details.endTime - m_details.startTime) * progress);
        }
        return {};
    }

    void onMouseMoved(sf::Vector2i mousePosition) override
    {
        const auto hovered = containsMouse(mousePosition);
        if (hovered != m_hovered) {
            m_hovered = hovered;
            updateHeight();
        }
    }

private:
    static constexpr auto fullHeight = 7;
    static constexpr auto minimizedHeight = 3;

    void updateGeometry() override
    {
        m_backgroundShape.setSize({size().x, getHeight()});
        m_filledShape.setSize({m_filledShape.getSize().x, getHeight()});
        updateHeight();
    };
    float getHeight() const { return m_hovered ? fullHeight : minimizedHeight; }
    void updateHeight()
    {
        auto updateShape = [&](auto &shape) {
            shape.setSize({shape.getSize().x, getHeight()});
            shape.setPosition({shape.getPosition().x, m_hovered ? 0.f : (fullHeight - minimizedHeight) / 2.f});
        };
        updateShape(m_backgroundShape);
        updateShape(m_filledShape);
    }

    ChapterDetails m_details;
    sf::RectangleShape m_backgroundShape;
    sf::RectangleShape m_filledShape;
    bool m_hovered{};
};

class SeekBar : public UiElement
{
public:
    explicit SeekBar(FilmTimer &timer)
        : m_timer{timer}
    {
        m_timer.onCurrnetTimeChanged([this](auto time) { setCurrentTime(time); });

        setSize({0, 16});
        setFillWidth(true);
    }

    void setFilmDetails(const FilmDetails &details)
    {
        m_filmDetails = details;
        updateChapters();
    }
    FilmDetails filmDetails() const { return m_filmDetails; }

    void setCurrentTime(std::chrono::milliseconds currentTime)
    {
        m_currentTime = currentTime;
        for (const auto &chapter : m_chapters) {
            chapter->setFilled(std::ranges::clamp(
                (m_currentTime.count() - chapter->details().startTime.count())
                    / float(chapter->details().duration().count()),
                0.0f,
                1.0f));
        }
    }

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override
    {
        for (const auto &shape : m_chapters) {
            target.draw(*shape.get());
        }
        UiElement::draw(target, states);
    }
    void onClicked(sf::Vector2i mousePosition) override
    {
        for (const auto &chapter : m_chapters) {
            if (const auto destination = chapter->tryJump(mousePosition)) {
                m_timer.jumpTo(destination.value());
            }
        }
    }
    void onMouseMoved(sf::Vector2i mousePosition) override
    {
        for (const auto &chapter : m_chapters) {
            chapter->onMouseMoved(mousePosition);
        }
    }

private:
    void updateChapters()
    {
        const auto newSize = m_filmDetails.chapters.size();
        const auto oldSize = m_chapters.size();
        if (newSize > m_filmDetails.chapters.size()) {
            m_chapters.erase(std::next(std::begin(m_chapters), newSize), std::end(m_chapters));
        } else if (oldSize < newSize) {
            for (auto i{oldSize}; i < newSize; ++i) {
                m_chapters.push_back(std::make_unique<Chapter>(m_filmDetails.chapters.at(i)));
            }
        }
    }

    void updateGeometry() override
    {
        const auto availableSize = size().x - (m_chapters.size() - 1) * m_spacing;
        for (auto x{0.f}; const auto &chapter : m_chapters) {
            const auto ratio = float(chapter->details().duration().count()) / m_filmDetails.duration.count();
            chapter->setPosition(getPosition() + sf::Vector2f{x, (size().y - chapter->size().y) / 2});
            chapter->setSize({ratio * availableSize, chapter->size().y});
            x += chapter->size().x + m_spacing;
        }
    }

    FilmTimer m_timer;
    FilmDetails m_filmDetails;
    std::chrono::milliseconds m_currentTime;
    int m_spacing{2};
    std::list<std::unique_ptr<Chapter>> m_chapters;
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

    void addEntry(std::unique_ptr<UiElement> &&entry) { m_entries.push_back(std::move(entry)); }

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override
    {
        for (const auto &entry : m_entries) {
            target.draw(*entry);
        }
        UiElement::draw(target, states);
    }

    void show() override
    {
        // updateMinimumSize();
        recalculateSizes();
        for (const auto &entry : m_entries) {
            entry->show();
        }
    }
    void onClicked(sf::Vector2i mousePosition) override
    {
        for (const auto &entry : m_entries) {
            entry->onClicked(mousePosition);
        }
    }
    void onMouseMoved(sf::Vector2i mousePosition) override
    {
        for (const auto &entry : m_entries) {
            entry->onMouseMoved(mousePosition);
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

        const auto sizePerSpacer = remainingSize / std::ranges::count_if(m_entries, [this](const auto &entry) {
                                       return entry->fillWidth() && m_orientation == Orientation::Horizontal
                                              || entry->fillHeight() && m_orientation == Orientation::Vertical;
                                   });
        for (const auto &entry : m_entries) {
            if (entry->fillWidth()) {
                entry->setSize(
                    {m_orientation == Orientation::Horizontal ? sizePerSpacer : size().x - 2 * m_padding,
                     entry->size().y});
            } else if (entry->fillHeight()) {
                entry->setSize(
                    {entry->size().x,
                     m_orientation == Orientation::Vertical ? sizePerSpacer : size().y - 2 * m_padding});
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

    sf::Vector2f m_size{};
    float m_spacing{};
    float m_padding{};
    Orientation m_orientation{};
    std::list<std::unique_ptr<UiElement>> m_entries;
};

int main()
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8.0;
    auto window = sf::RenderWindow({600, 300}, "yt seeker", sf::Style::Default, settings);
    window.setFramerateLimit(144);

    FilmTimer filmTimer;

    Layout layout{Orientation::Vertical};
    layout.setSize(sf::Vector2f{window.getSize()});
    layout.setSpacing(8);
    layout.setPadding(20);
    layout.addEntry(std::make_unique<VSpacer>());
    auto seekBar = std::make_unique<SeekBar>(filmTimer);
    seekBar->setFilmDetails(
        {.name = "test",
         .duration = std::chrono::seconds{100},
         .chapters
         = {{.name = "Chapter 1", .startTime = std::chrono::seconds{0}, .endTime = std::chrono::seconds{20}},
            {.name = "Chapter 2", .startTime = std::chrono::seconds{20}, .endTime = std::chrono::seconds{80}},
            {.name = "Chapter 3", .startTime = std::chrono::seconds{80}, .endTime = std::chrono::seconds{100}}}});
    layout.addEntry(std::move(seekBar));
    layout.addEntry(std::make_unique<PlayButton>([&](auto playing) {
        if (playing) {
            filmTimer.play();
        } else {
            filmTimer.stop();
        }
    }));
    layout.show();

    sf::Clock clock;
    while (window.isOpen()) {
        for (auto event = sf::Event(); window.pollEvent(event);) {
            const auto mousePos = sf::Mouse::getPosition(window);
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::MouseMoved) {
                layout.onMouseMoved(mousePos);
            } else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                layout.onClicked(mousePos);
            } else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Left) {
                    filmTimer.jumpBackwards();
                } else if (event.key.code == sf::Keyboard::Right) {
                    filmTimer.jumpForward();
                }
            }
        }
        filmTimer.update();
        window.clear(sf::Color{37, 38, 40});
        window.draw(layout);

        window.display();
    }
}
