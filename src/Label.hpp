#pragma once

#include "UiElement.hpp"

class Label : public UiElement
{
public:
    explicit Label();
    virtual ~Label() = default;

    std::string text() const;
    void setText(const std::string &text);

    sf::FloatRect getGlobalBounds() const;

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

private:
    sf::Text m_text;
};
