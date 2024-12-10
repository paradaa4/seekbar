#include "FilmController.hpp"
#include <gtest/gtest.h>
#include <thread>

constexpr auto FilmDuration = std::chrono::seconds{60};
auto createController = [](FilmDetails details = {.name = "Test", .duration = FilmDuration}) {
    return FilmController{details};
};
TEST(FilmController, state)
{
    auto controller = createController();
    auto notifiedCounter = 0;
    controller.onStateChanged([&] { ++notifiedCounter; });
    ASSERT_EQ(controller.state(), FilmController::State::Loading);
    ASSERT_EQ(notifiedCounter, 0);
    controller.play();
    ASSERT_EQ(controller.state(), FilmController::State::Playing);
    ASSERT_EQ(notifiedCounter, 1);
    controller.pause();
    ASSERT_EQ(controller.state(), FilmController::State::Paused);
    ASSERT_EQ(notifiedCounter, 2);
}

TEST(FilmController, play)
{
    auto controller = createController();
    ASSERT_FALSE(controller.playing());
    controller.play();
    ASSERT_TRUE(controller.playing());
    controller.play();
    ASSERT_TRUE(controller.playing());
    controller.pause();
    ASSERT_FALSE(controller.playing());
}

TEST(FilmController, pause)
{
    auto controller = createController();
    ASSERT_FALSE(controller.paused());
    controller.pause();
    ASSERT_TRUE(controller.paused());
    controller.pause();
    ASSERT_TRUE(controller.paused());
    controller.play();
    ASSERT_FALSE(controller.paused());
}

TEST(FilmController, restart)
{
    auto controller = createController();
    controller.play();
    ASSERT_EQ(controller.currentTime(), std::chrono::seconds{0});
    controller.jumpForward();
    ASSERT_NE(controller.currentTime(), std::chrono::seconds{0});
    controller.restart();
    ASSERT_EQ(controller.currentTime(), std::chrono::seconds{0});
}

TEST(FilmController, atEnd)
{
    auto controller = createController();
    controller.play();
    ASSERT_FALSE(controller.atEnd());
    controller.jumpTo(std::chrono::seconds{5});
    ASSERT_FALSE(controller.atEnd());
    controller.jumpTo(FilmDuration);
    ASSERT_TRUE(controller.atEnd());
}

TEST(FilmController, jumpBackward)
{
    auto controller = createController();
    controller.play();
    ASSERT_EQ(controller.currentTime(), std::chrono::seconds{0});
    controller.jumpBackward();
    ASSERT_EQ(controller.currentTime(), std::chrono::seconds{0});
    controller.jumpTo(std::chrono::seconds{30});
    ASSERT_EQ(controller.currentTime(), std::chrono::seconds{30});
    controller.jumpBackward();
    ASSERT_EQ(controller.currentTime(), std::chrono::seconds{20});
    controller.jumpBackward();
    ASSERT_EQ(controller.currentTime(), std::chrono::seconds{10});
    controller.jumpBackward();
    ASSERT_EQ(controller.currentTime(), std::chrono::seconds{0});
}

TEST(FilmController, jumpForward)
{
    auto controller = createController();
    controller.play();
    ASSERT_EQ(controller.currentTime(), std::chrono::seconds{0});
    controller.jumpForward();
    ASSERT_EQ(controller.currentTime(), std::chrono::seconds{10});
    controller.jumpForward();
    ASSERT_EQ(controller.currentTime(), std::chrono::seconds{20});
    controller.jumpTo(FilmDuration);
    ASSERT_EQ(controller.currentTime(), FilmDuration);
    controller.jumpForward();
    ASSERT_EQ(controller.currentTime(), FilmDuration);
}

TEST(FilmController, update)
{
    auto controller = createController();
    controller.play();
    ASSERT_EQ(controller.currentTime(), std::chrono::seconds{0});
    std::this_thread::sleep_for(std::chrono::milliseconds{100});
    controller.update();
    ASSERT_GE(controller.currentTime(), std::chrono::milliseconds{100});
    std::this_thread::sleep_for(std::chrono::milliseconds{200});
    controller.update();
    ASSERT_GE(controller.currentTime(), std::chrono::milliseconds{300});
}