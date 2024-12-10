#include "CurrrentTimeLabel.hpp"
#include <format>

CurrentTimeLabel::CurrentTimeLabel(FilmController &controller)
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
            m_controller.loading()
                ? ""
                : std::format(
                    "{} / {}", formatTime(m_controller.currentTime()), formatTime(m_controller.filmDetails().duration)));
    };
    m_controller.onCurrentTimeChanged(updateText);
    m_controller.onStateChanged(updateText);
    updateText();
}
