#pragma once

#include "UiElement.hpp"

class Spacer : public UiElement
{
public:
    explicit Spacer(const std::optional<float> maxSize = {}, Orientation orientation = {});
};

class VSpacer : public Spacer
{
public:
    explicit VSpacer(const std::optional<float> maxSize = {});
};

class HSpacer : public Spacer
{
public:
    explicit HSpacer(const std::optional<float> maxSize = {});
};