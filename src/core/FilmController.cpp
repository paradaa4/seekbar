#include "FilmController.hpp"
#include <numeric>

constexpr auto JumpInterval = std::chrono::seconds{10};

FilmController::FilmController(const FilmDetails &details)
    : m_filmDetails{details}
{}

FilmController::State FilmController::state() const
{
    return m_state;
}

FilmDetails FilmController::filmDetails() const
{
    return m_filmDetails;
}

std::chrono::milliseconds FilmController::currentTime() const
{
    return m_currentTime;
}

bool FilmController::playing() const
{
    return m_state == State::Playing;
}

bool FilmController::paused() const
{
    return m_state == State::Paused;
}

bool FilmController::loading() const
{
    return m_state == State::Loading;
}

bool FilmController::atEnd() const
{
    return m_currentTime == m_filmDetails.duration;
}

void FilmController::play()
{
    if (playing()) {
        return;
    }
    m_state = State::Playing;
    m_clock.restart();
    notify(m_stateChangedCallbacks);
}

void FilmController::pause()
{
    if (paused()) {
        return;
    }
    m_state = State::Paused;
    notify(m_stateChangedCallbacks);
}

void FilmController::restart()
{
    jump(-m_currentTime);
    play();
}

void FilmController::jumpBackwards()
{
    jump(-JumpInterval);
}

void FilmController::jumpForward()
{
    jump(JumpInterval);
}

void FilmController::jumpTo(std::chrono::milliseconds time)
{
    jump(time - m_currentTime);
}

void FilmController::update()
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

void FilmController::onCurrentTimeChanged(Callback &&callback)
{
    m_currentTimeChangedCallbacks.push_back(std::move(callback));
}

void FilmController::onStateChanged(Callback &&callback)
{
    m_stateChangedCallbacks.push_back(std::move(callback));
}

void FilmController::jump(std::chrono::milliseconds interval)
{
    if (loading()) {
        return;
    }
    m_currentTime = std::clamp(m_currentTime + interval, std::chrono::milliseconds{0}, m_filmDetails.duration);
    update();
    notify(m_currentTimeChangedCallbacks);
}

void FilmController::notify(const std::list<Callback> &callbacks)
{
    std::for_each(std::cbegin(callbacks), std::cend(callbacks), [](auto &c) { c(); });
}