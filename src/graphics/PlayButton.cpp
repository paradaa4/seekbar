#include "PlayButton.hpp"
#include <numbers>
#include <ranges>

constexpr auto Padding = 2.f;
constexpr auto LoadingCirclesCount = 10;
constexpr auto DefaultSize = 20.f;
constexpr auto CircleRadius = 2.f;

PlayButton::PlayButton(FilmController &controller)
    : m_controller{controller}
{
    setSize({DefaultSize, DefaultSize});

    m_playShape = [] {
        auto shape = std::make_unique<sf::ConvexShape>();
        shape->setPointCount(3);
        shape->setPoint(0, {Padding, Padding});
        shape->setPoint(1, {Padding, DefaultSize - Padding});
        shape->setPoint(2, {DefaultSize - Padding * 2, DefaultSize / 2});
        return shape;
    }();

    m_restartShape = [] {
        auto shape = std::make_unique<sf::RectangleShape>();
        shape->setSize({DefaultSize - 2 * Padding, DefaultSize - 2 * Padding});
        shape->setPosition({Padding, Padding});
        return shape;
    }();

    auto createPauseShape = [this](int offset = 0) {
        auto shape = std::make_unique<sf::RectangleShape>();
        shape->setPosition({Padding + offset, Padding});
        shape->setSize({2 * Padding, DefaultSize - 2 * Padding});
        return shape;
    };
    m_pausedShapes.push_back(createPauseShape());
    m_pausedShapes.push_back(createPauseShape(10));

    auto createLoadingShape = [this](int i = 0) {
        static auto increment = 2 * std::numbers::pi / LoadingCirclesCount;
        static auto startAngle = -std::numbers::pi / 2;

        auto shape = std::make_unique<sf::CircleShape>();
        shape->setRadius(CircleRadius);
        shape->setPosition(
            {DefaultSize / 2 - CircleRadius + DefaultSize / 2 * float(std::cos(startAngle + i * increment)),
             DefaultSize / 2 - CircleRadius + DefaultSize / 2 * float(std::sin(startAngle + i * increment))});
        shape->setFillColor(
            sf::Color::White
            - sf::Color{0, 0, 0, sf::Uint8(std::numeric_limits<sf::Uint8>::max() * (i / float(LoadingCirclesCount)))});
        return shape;
    };
    m_loadingShapes = std::views::iota(0, LoadingCirclesCount)
                      | std::views::transform([&](auto i) { return createLoadingShape(i); })
                      | std::ranges::to<std::vector>();
}

void PlayButton::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    states.transform *= getTransform();

    if (m_controller.playing()) {
        for (const auto &shape : m_pausedShapes) {
            target.draw(*shape.get(), states);
        }
    } else if (m_controller.loading()) {
        states.transform.rotate(m_clock.getElapsedTime().asMilliseconds() / 2 % 360, DefaultSize / 2, DefaultSize / 2);
        for (const auto &shape : m_loadingShapes) {
            target.draw(*shape.get(), states);
        }
    } else if (m_controller.atEnd()) {
        target.draw(*m_restartShape.get(), states);

    } else {
        target.draw(*m_playShape.get(), states);
    }
}

void PlayButton::onPressed(sf::Vector2i mousePosition)
{
    if (m_controller.playing()) {
        m_controller.pause();
    } else if (m_controller.atEnd()) {
        m_controller.restart();
    } else if (m_controller.paused()) {
        m_controller.play();
    }
}
