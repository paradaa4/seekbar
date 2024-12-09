#include <SFML/Graphics.hpp>
#include <chrono>
#include <format>
#include <functional>
#include <iostream>
#include <list>
#include <numbers>
#include <numeric>
#include <random>
#include <ranges>

auto format(auto v)
{
    return std::format("({}, {})\n", v.x, v.y);
}

enum class Orientation { Horizontal, Vertical };

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

class FilmController
{
public:
    using Callback = std::function<void()>;

    enum class State { Playing, Paused, Loading };

    void setFilmDetails(const FilmDetails &details) { m_filmDetails = details; }
    FilmDetails filmDetails() const { return m_filmDetails; }

    std::chrono::milliseconds currentTime() const { return m_currentTime; }
    bool playing() const { return m_state == State::Playing; }
    bool paused() const { return m_state == State::Paused; }
    bool loading() const { return m_state == State::Loading; }
    bool atEnd() const { return m_currentTime == m_filmDetails.duration; }
    void play()
    {
        if (playing()) {
            return;
        }
        m_state = State::Playing;
        m_clock.restart();
        notify(m_stateChangedCallbacks);
    }
    void pause()
    {
        if (paused()) {
            return;
        }
        m_state = State::Paused;
        notify(m_stateChangedCallbacks);
    }
    void restart()
    {
        jump(-m_currentTime);
        play();
    }
    void jumpBackwards() { jump(-seekInterval); }
    void jumpForward() { jump(seekInterval); }
    void jumpTo(auto time) { jump(time - m_currentTime); }
    void update()
    {
        if (playing()) {
            m_currentTime += std::chrono::milliseconds{m_clock.getElapsedTime().asMilliseconds()};
            if (m_currentTime > m_filmDetails.duration) {
                m_currentTime = m_filmDetails.duration;
                pause();
            }
            m_clock.restart();
            notify(m_currentTimeChangedCallbacks);
        }
    }

    void onCurrentTimeChanged(Callback &&callback) { m_currentTimeChangedCallbacks.push_back(std::move(callback)); }
    void onStateChanged(Callback &&callback) { m_stateChangedCallbacks.push_back(std::move(callback)); };

private:
    static constexpr auto seekInterval = std::chrono::seconds{10};

    void jump(auto interval)
    {
        if (loading()) {
            return;
        }
        m_currentTime
            = std::ranges::clamp(m_currentTime + interval, std::chrono::milliseconds{0}, m_filmDetails.duration);
        notify(m_currentTimeChangedCallbacks);
    }

    void notify(const std::list<Callback> &callbacks)
    {
        std::for_each(std::cbegin(callbacks), std::cend(callbacks), [](auto &c) { c(); });
    }

    FilmDetails m_filmDetails;
    std::list<Callback> m_currentTimeChangedCallbacks;
    std::list<Callback> m_stateChangedCallbacks;
    State m_state{State::Loading};
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
        states.transform *= getTransform();

        // sf::RectangleShape background{size()};
        // background.setPosition(getPosition());
        // background.setFillColor(m_debugBackgroundColor);
        // target.draw(background, states);

        if (m_shape) {
            target.draw(*m_shape.get(), states);
        }
    }

    virtual void show() { updateGeometry(); }

    virtual void handleMousePressed(sf::Vector2i mousePosition)
    {
        if (containsMouse(mousePosition)) {
            m_pressed = true;
            onPressed(mousePosition);
        }
    }
    virtual void handleMouseReleased(sf::Vector2i mousePosition)
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
    virtual void handleMouseMoved(sf::Vector2i mousePosition)
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

    bool pressed() const { return m_pressed; }
    bool hovered() const { return m_hovered; }
    bool dragged() const { return m_dragged; }

protected:
    sf::Rect<float> rect() const { return {getPosition(), m_size}; }
    bool containsMouse(sf::Vector2i mousePosition) const { return rect().contains(sf::Vector2f{mousePosition}); }

private:
    virtual void updateGeometry(){};
    virtual void onPressed(sf::Vector2i mousePosition) {}
    virtual void onReleased() {}
    virtual void onHoveredChanged() {}
    virtual void onDragStarted() {}
    virtual void onDragFinished() {}
    virtual void onDragMove(sf::Vector2i mousePosition) {}

    sf::Vector2f m_size{};
    bool m_fillWidth{};
    bool m_fillHeight{};
    std::unique_ptr<sf::Shape> m_shape;
    bool m_pressed{};
    bool m_hovered{};
    bool m_dragged{};
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
    explicit HSpacer(const std::optional<float> maxWidth = {})
    {
        if (maxWidth) {
            setSize({maxWidth.value(), 0});
        } else {
            setFillWidth(true);
        }
    }
};
class PlayButton : public UiElement
{
public:
    explicit PlayButton(FilmController &controller)
        : m_controller{controller}
    {
        setSize({20, 20});
        m_playShape = std::make_unique<sf::ConvexShape>();
        m_playShape->setPointCount(3);
        m_playShape->setPoint(0, {padding, padding});
        m_playShape->setPoint(1, {padding, size().y - padding});
        m_playShape->setPoint(2, {size().x - padding * 2, size().y / 2});
        m_restartShape = std::make_unique<sf::RectangleShape>();
        m_restartShape->setSize({size().x - 2 * padding, size().y - 2 * padding});
        m_restartShape->setPosition({padding, padding});
        auto createPauseShape = [this](int offset = 0) {
            auto shape = std::make_unique<sf::RectangleShape>();
            shape->setPosition({padding + offset, padding});
            shape->setSize({2 * padding, size().y - 2 * padding});
            return shape;
        };
        m_pausedShapes.push_back(createPauseShape());
        m_pausedShapes.push_back(createPauseShape(10));

        auto createLoadingShape = [this](int i = 0) {
            static auto increment = 2 * std::numbers::pi / loadingCirclesCount;
            static auto startAngle = -std::numbers::pi / 2;

            auto shape = std::make_unique<sf::CircleShape>();
            shape->setRadius(2);
            shape->setPosition(
                {size().x / 2 - shape->getRadius() + size().x / 2 * float(std::cos(startAngle + i * increment)),
                 size().y / 2 - shape->getRadius() + size().y / 2 * float(std::sin(startAngle + i * increment))});
            shape->setFillColor(
                sf::Color::White - sf::Color{0, 0, 0, sf::Uint8(255 * (i / float(loadingCirclesCount)))});
            return shape;
        };
        m_loadingShapes = std::views::iota(0, loadingCirclesCount)
                          | std::views::transform([&](auto i) { return createLoadingShape(i); })
                          | std::ranges::to<std::vector>();
    }

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override
    {
        states.transform *= getTransform();

        if (m_controller.playing()) {
            for (const auto &shape : m_pausedShapes) {
                target.draw(*shape.get(), states);
            }
        } else if (m_controller.loading()) {
            states.transform.rotate(m_clock.getElapsedTime().asMilliseconds() / 2 % 360, size().x / 2, size().y / 2);
            for (const auto &shape : m_loadingShapes) {
                target.draw(*shape.get(), states);
            }
        } else if (m_controller.atEnd()) {
            target.draw(*m_restartShape.get(), states);

        } else {
            target.draw(*m_playShape.get(), states);
        }
    }

private:
    static constexpr auto padding = 2.f;
    static constexpr auto loadingCirclesCount = 10;

    void onPressed(sf::Vector2i mousePosition) override
    {
        if (m_controller.loading()) {
            return;
        }
        if (m_controller.playing()) {
            m_controller.pause();
        } else if (m_controller.atEnd()) {
            m_controller.restart();
        } else {
            m_controller.play();
        }
    }

    FilmController &m_controller;
    std::vector<std::unique_ptr<sf::RectangleShape>> m_pausedShapes;
    std::vector<std::unique_ptr<sf::CircleShape>> m_loadingShapes;
    std::unique_ptr<sf::ConvexShape> m_playShape;
    std::unique_ptr<sf::RectangleShape> m_restartShape;
    sf::Clock m_clock;
};

class Label : public UiElement
{
public:
    explicit Label()
    {
        static auto font = [&]() {
            sf::Font font;
            font.loadFromFile(fontPath);
            return font;
        }();
        m_text.setFillColor(sf::Color::White);
        m_text.setFont(font);
        m_text.setCharacterSize(16);
    }
    void setText(const std::string &text) { m_text.setString(text); }

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override
    {
        states.transform *= getTransform();
        target.draw(m_text, states);
    }

    auto getGlobalBounds() const { return m_text.getGlobalBounds(); }

private:
    static constexpr auto fontPath = "fonts/Arial.ttf";

    sf::Text m_text;
};

class Chapter : public UiElement
{
public:
    explicit Chapter(const ChapterDetails &details)
        : m_details{details}
    {
        m_backgroundShape.setFillColor(sf::Color{180, 180, 180, 100});
        m_filledShape.setFillColor(sf::Color::Red);
        m_label.setText(m_details.name);
    }

    ChapterDetails details() const { return m_details; }

    void setFilled(float filled) { m_filledShape.setSize({filled * size().x, getHeight()}); }

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override
    {
        states.transform *= getTransform();
        target.draw(m_backgroundShape, states);
        target.draw(m_filledShape, states);
        if (hovered()) {
            target.draw(m_label, states);
        }
    }

private:
    static constexpr auto fullHeight = 7;
    static constexpr auto minimizedHeight = 3;
    static constexpr auto fontPath = "fonts/Arial.ttf";

    void updateGeometry() override
    {
        m_backgroundShape.setSize({size().x, getHeight()});
        m_filledShape.setSize({m_filledShape.getSize().x, getHeight()});
        m_label.setPosition(
            size().x / 2 - m_label.getGlobalBounds().width / 2,
            size().y / 2 - m_label.getGlobalBounds().height / 2 - fullHeight * 3);

        updateHeight();
    };
    void onHoveredChanged() override { updateHeight(); }
    float getHeight() const { return hovered() ? fullHeight : minimizedHeight; }
    void updateHeight()
    {
        auto updateShape = [&](auto &shape) {
            shape.setSize({shape.getSize().x, getHeight()});
            shape.setPosition({shape.getPosition().x, (size().y - (hovered() ? fullHeight : minimizedHeight)) / 2.f});
        };
        updateShape(m_backgroundShape);
        updateShape(m_filledShape);
    }

    ChapterDetails m_details;
    sf::RectangleShape m_backgroundShape;
    sf::RectangleShape m_filledShape;
    Label m_label;
};

class SeekBar : public UiElement
{
public:
    explicit SeekBar(FilmController &controller)
        : m_controller{controller}
    {
        m_controller.onCurrentTimeChanged([this] { setCurrentTime(m_controller.currentTime()); });

        setSize({0, 16});
        setFillWidth(true);
        m_handle.setRadius(6);
        m_handle.setFillColor(sf::Color::Red);
        m_handle.setPosition({-m_handle.getRadius(), size().y / 2 - m_handle.getRadius()});

        updateChapters();
    }

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
        m_handle.setPosition(
            {size().x * m_controller.currentTime().count() / m_controller.filmDetails().duration.count()
                 - m_handle.getRadius(),
             size().y / 2 - m_handle.getRadius()});
    }

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override
    {
        states.transform *= getTransform();
        if (m_controller.loading()) {
            return;
        }
        for (const auto &shape : m_chapters) {
            target.draw(*shape.get(), states);
        }
        if (hovered() || pressed()) {
            target.draw(m_handle, states);
        }

        UiElement::draw(target, states);
    }
    void handleMouseMoved(sf::Vector2i mousePosition) override
    {
        if (m_controller.loading()) {
            return;
        }
        UiElement::handleMouseMoved(mousePosition);
        mousePosition -= sf::Vector2i{getPosition()};
        for (const auto &chapter : m_chapters) {
            chapter->handleMouseMoved(mousePosition);
        }
    }

private:
    void updateGeometry() override
    {
        const auto availableSize = size().x - (m_chapters.size() - 1) * m_spacing;
        for (auto x{0.f}; const auto &chapter : m_chapters) {
            const auto ratio = float(chapter->details().duration().count())
                               / m_controller.filmDetails().duration.count();
            chapter->setPosition(sf::Vector2f{x, (size().y - chapter->size().y) / 2});
            chapter->setSize({ratio * availableSize, size().y});
            x += chapter->size().x + m_spacing;
        }
    }
    void onPressed(sf::Vector2i mousePosition) override
    {
        if (m_controller.loading()) {
            return;
        }
        m_controller.jumpTo(std::chrono::duration_cast<std::chrono::milliseconds>(
            m_controller.filmDetails().duration * (mousePosition.x - getPosition().x) / size().x));
    }
    void onDragStarted() override
    {
        if (m_wasPlaying = m_controller.playing(); m_wasPlaying) {
            m_controller.pause();
        }
    }
    void onDragFinished() override
    {
        if (m_wasPlaying) {
            m_controller.play();
        }
    }
    void onDragMove(sf::Vector2i mousePosition) override { onPressed(mousePosition); }

    void updateChapters()
    {
        const auto newSize = m_controller.filmDetails().chapters.size();
        const auto oldSize = m_chapters.size();
        if (newSize > m_controller.filmDetails().chapters.size()) {
            m_chapters.erase(std::next(std::begin(m_chapters), newSize), std::end(m_chapters));
        } else if (oldSize < newSize) {
            for (auto i{oldSize}; i < newSize; ++i) {
                m_chapters.push_back(std::make_unique<Chapter>(m_controller.filmDetails().chapters.at(i)));
            }
        }
    }

    FilmController &m_controller;
    std::chrono::milliseconds m_currentTime;
    std::list<std::unique_ptr<Chapter>> m_chapters;
    sf::CircleShape m_handle;
    bool m_wasPlaying{};
    int m_spacing{2};
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
        recalculateSizes();
        for (const auto &entry : m_entries) {
            entry->show();
        }
    }
    void handleMousePressed(sf::Vector2i mousePosition) override
    {
        for (const auto &entry : m_entries) {
            entry->handleMousePressed(mousePosition);
        }
    }
    void handleMouseReleased(sf::Vector2i mousePosition) override
    {
        for (const auto &entry : m_entries) {
            entry->handleMouseReleased(mousePosition);
        }
    }
    void handleMouseMoved(sf::Vector2i mousePosition) override
    {
        mousePosition -= sf::Vector2i{getPosition()};
        for (const auto &entry : m_entries) {
            entry->handleMouseMoved(mousePosition);
        }
    }

private:
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

class CurrentTimeLabel : public Label
{
public:
    explicit CurrentTimeLabel(FilmController &controller)
        : m_controller{controller}
    {
        auto updateText = [this] {
            auto formatTime = [](auto time) {
                return std::format(
                    "{}:{:0>2}",
                    std::chrono::duration_cast<std::chrono::minutes>(time).count(),
                    std::chrono::duration_cast<std::chrono::seconds>(time).count() % 60);
            };
            setText(
                m_controller.loading() ? ""
                                       : std::format(
                                           "{} / {}",
                                           formatTime(m_controller.currentTime()),
                                           formatTime(m_controller.filmDetails().duration)));
        };
        m_controller.onCurrentTimeChanged(updateText);
        m_controller.onStateChanged(updateText);
        updateText();
    }

private:
    FilmController &m_controller;
};

int main()
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8.0;
    auto window = sf::RenderWindow({600, 300}, "yt seeker", sf::Style::Resize | sf::Style::Close, settings);
    window.setFramerateLimit(144);

    FilmController filmController;
    filmController.setFilmDetails(
        {.name = "test",
         .duration = std::chrono::seconds{100},
         .chapters
         = {{.name = "Intro", .startTime = std::chrono::seconds{0}, .endTime = std::chrono::seconds{10}},
            {.name = "Explanation", .startTime = std::chrono::seconds{10}, .endTime = std::chrono::seconds{70}},
            {.name = "Summary", .startTime = std::chrono::seconds{70}, .endTime = std::chrono::seconds{85}},
            {.name = "Goodbye", .startTime = std::chrono::seconds{85}, .endTime = std::chrono::seconds{100}}}});

    Layout layout{Orientation::Vertical};
    layout.setSize(sf::Vector2f{window.getSize()});
    layout.setSpacing(4);
    layout.setPadding(10);
    layout.addEntry(std::make_unique<VSpacer>());
    layout.addEntry(std::make_unique<SeekBar>(filmController));
    auto hLayout = std::make_unique<Layout>(Orientation::Horizontal);
    hLayout->setSize({0, 20});
    hLayout->addEntry(std::make_unique<PlayButton>(filmController));
    hLayout->addEntry(std::make_unique<HSpacer>(20));
    hLayout->addEntry(std::make_unique<CurrentTimeLabel>(filmController));
    hLayout->addEntry(std::make_unique<HSpacer>());
    layout.addEntry(std::move(hLayout));
    layout.show();

    sf::Clock loadingClock;
    while (window.isOpen()) {
        for (auto event = sf::Event(); window.pollEvent(event);) {
            const auto mousePosition = sf::Mouse::getPosition(window);
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::MouseMoved) {
                layout.handleMouseMoved(mousePosition);
            } else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                layout.handleMousePressed(mousePosition);
            } else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                layout.handleMouseReleased(mousePosition);
            } else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Left) {
                    filmController.jumpBackwards();
                } else if (event.key.code == sf::Keyboard::Right) {
                    filmController.jumpForward();
                } else if (event.key.code == sf::Keyboard::Space && !filmController.loading()) {
                    if (filmController.playing()) {
                        filmController.pause();
                    } else {
                        filmController.play();
                    }
                }
            }
        }
        if (loadingClock.getElapsedTime() > sf::seconds(3) && filmController.loading()) {
            filmController.pause();
        }
        filmController.update();
        window.clear(sf::Color{37, 38, 40});
        window.draw(layout);
        window.display();
    }
}
