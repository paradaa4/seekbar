#pragma once

#include <chrono>
#include <string>
#include <vector>

struct FilmDetails
{
    struct ChapterDetails
    {
        std::string name;
        std::chrono::milliseconds startTime{};
        std::chrono::milliseconds endTime{};

        std::chrono::milliseconds duration() { return endTime - startTime; }
    };

    std::string name;
    std::chrono::milliseconds duration{};
    std::vector<ChapterDetails> chapters;
};
