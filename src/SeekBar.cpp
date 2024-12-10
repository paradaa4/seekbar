#include "SeekBar.hpp"

const auto DefaultSize = sf::Vector2f{0, 16};
constexpr auto HandleRadius = 6.f;
const auto HandleColor = sf::Color{240, 50, 50};

SeekBar::SeekBar(FilmController &controller)
    : m_controller{controller}
{
    setSize(DefaultSize);
    setFillWidth(true);

    m_handle.setRadius(HandleRadius);
    m_handle.setFillColor(HandleColor);
    m_handle.setPosition({-HandleRadius, size().y / 2 - HandleRadius});

    updateChapters();
    m_controller.onCurrentTimeChanged([this] { setCurrentTime(m_controller.currentTime()); });
}

void SeekBar::draw(sf::RenderTarget &target, sf::RenderStates states) const
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
void SeekBar::handleMouseMoved(sf::Vector2i mousePosition)
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

void SeekBar::updateGeometry()
{
    const auto availableSize = size().x - (m_chapters.size() - 1) * m_spacing;
    for (auto x{0.f}; const auto &chapter : m_chapters) {
        const auto ratio = float(chapter->details().duration().count()) / m_controller.filmDetails().duration.count();
        chapter->setPosition(sf::Vector2f{x, (size().y - chapter->size().y) / 2});
        chapter->setSize({ratio * availableSize, size().y});
        x += chapter->size().x + m_spacing;
    }
}

void SeekBar::onPressed(sf::Vector2i mousePosition)
{
    if (m_controller.loading()) {
        return;
    }
    m_controller.jumpTo(std::chrono::duration_cast<std::chrono::milliseconds>(
        m_controller.filmDetails().duration * (mousePosition.x - getPosition().x) / size().x));
}

void SeekBar::onDragStarted()
{
    if (m_wasPlaying = m_controller.playing(); m_wasPlaying) {
        m_controller.pause();
    }
}

void SeekBar::onDragFinished()
{
    if (m_wasPlaying) {
        m_controller.play();
    }
}

void SeekBar::onDragMove(sf::Vector2i mousePosition)
{
    onPressed(mousePosition);
}

void SeekBar::updateChapters()
{
    const auto newSize = m_controller.filmDetails().chapters.size();
    const auto oldSize = m_chapters.size();
    if (newSize > m_controller.filmDetails().chapters.size()) {
        m_chapters.erase(std::next(std::begin(m_chapters), newSize), std::end(m_chapters));
    } else if (oldSize < newSize) {
        std::generate_n(std::back_inserter(m_chapters), newSize - oldSize, [this, i = oldSize] mutable {
            return std::make_unique<Chapter>(m_controller.filmDetails().chapters.at(i++));
        });
    }
}

void SeekBar::setCurrentTime(std::chrono::milliseconds currentTime)
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
        {size().x * m_controller.currentTime().count() / m_controller.filmDetails().duration.count() - HandleRadius,
         size().y / 2 - HandleRadius});
}