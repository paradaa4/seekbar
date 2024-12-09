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

add_executable(yt_seeker src/main.cpp)
set_target_properties(yt_seeker PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
target_link_libraries(yt_seeker PRIVATE sfml-graphics)