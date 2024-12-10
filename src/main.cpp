
#include "Application.hpp"

int main()
{
    FilmController filmController{
        {.name = "test",
         .duration = std::chrono::seconds{100},
         .chapters
         = {{.name = "Intro", .startTime = std::chrono::seconds{0}, .endTime = std::chrono::seconds{10}},
            {.name = "Explanation", .startTime = std::chrono::seconds{10}, .endTime = std::chrono::seconds{70}},
            {.name = "Summary", .startTime = std::chrono::seconds{70}, .endTime = std::chrono::seconds{85}},
            {.name = "Goodbye", .startTime = std::chrono::seconds{85}, .endTime = std::chrono::seconds{100}}}}};

    Application app{filmController};
    app.run();
    return 0;
}
