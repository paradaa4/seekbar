cmake_minimum_required(VERSION 3.26)
project(yt_seeker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)
FetchContent_Declare(SFML
GIT_REPOSITORY https://github.com/SFML/SFML.git
GIT_TAG 2.6.x
GIT_SHALLOW ON
EXCLUDE_FROM_ALL
SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(yt_seeker 
    src/main.cpp
    src/Chapter.cpp
    src/Chapter.hpp
    src/CurrrentTimeLabel.cpp
    src/CurrrentTimeLabel.hpp
    src/FilmController.cpp
    src/FilmController.hpp
    src/FilmDetails.hpp
    src/Label.cpp
    src/Label.hpp
    src/Layout.cpp
    src/Layout.hpp
    src/PlayButton.cpp
    src/PlayButton.hpp
    src/SeekBar.cpp
    src/SeekBar.hpp
    src/Spacer.cpp
    src/Spacer.hpp
    src/Types.hpp
    src/UiElement.cpp
    src/UiElement.hpp)
target_link_libraries(yt_seeker PRIVATE sfml-graphics)