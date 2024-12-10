add_subdirectory(core)
add_subdirectory(graphics)

add_executable(yt_seeker 
    main.cpp
    Application.cpp
    Application.hpp)
target_link_libraries(yt_seeker PRIVATE core graphics sfml-graphics)