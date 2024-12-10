#include "Label.hpp"
#include <filesystem>

constexpr auto FontsDirectory = "fonts";
constexpr auto FontName = "Arial.ttf";
constexpr auto FontSize = 16;

Label::Label()
{
    static auto font = [&]() {
        sf::Font font;
        auto path = std::filesystem::path(std::filesystem::current_path() / FontsDirectory);
        if (!std::filesystem::exists(path)) {
            path = std::filesystem::current_path().parent_path() / FontsDirectory;
        }
        font.loadFromFile(path / FontName);
        return font;
    }();
    m_text.setFillColor(sf::Color::White);
    m_text.setFont(font);
    m_text.setCharacterSize(FontSize);
}

std::string Label::text() const
{
    return m_text.getString();
}

void Label::setText(const std::string &text)
{
    m_text.setString(text);
}

sf::FloatRect Label::getGlobalBounds() const
{
    return m_text.getGlobalBounds();
}

void Label::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    target.draw(m_text, states);
}
