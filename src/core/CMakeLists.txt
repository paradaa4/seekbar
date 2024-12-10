add_library(core)

target_sources(core
  PRIVATE
    FilmController.cpp
    FilmController.hpp
    FilmDetails.hpp
)
target_link_libraries(core PUBLIC sfml-graphics)
target_include_directories(core
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}    
)
