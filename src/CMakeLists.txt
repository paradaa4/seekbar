include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

add_subdirectory(core)
add_subdirectory(graphics)

add_executable(seekbar 
    main.cpp
    Application.cpp
    Application.hpp)
target_link_libraries(seekbar PRIVATE graphics)