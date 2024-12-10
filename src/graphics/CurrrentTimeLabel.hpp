#pragma once

#include "FilmController.hpp"
#include "Label.hpp"

class CurrentTimeLabel : public Label
{
public:
    explicit CurrentTimeLabel(FilmController &controller);

private:
    FilmController &m_controller;
};