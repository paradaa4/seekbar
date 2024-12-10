#pragma once

#include "UiElement.hpp"

class Label : public UiElement
{
public:
    explicit Label();
    virtual ~Label() = default;

    void setText(const std::string &text);

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

    sf::FloatRect getGlobalBounds() const;

private:
    sf::Text m_text;
};
