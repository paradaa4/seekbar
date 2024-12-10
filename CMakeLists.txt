cmake_minimum_required(VERSION 3.26)
project(seekbar LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory(src)

enable_testing()
add_subdirectory(test)