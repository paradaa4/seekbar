add_library(graphics)

target_sources(graphics
  PRIVATE
    Chapter.cpp
    Chapter.hpp
    CurrrentTimeLabel.cpp
    CurrrentTimeLabel.hpp
    Label.cpp
    Label.hpp
    Layout.cpp
    Layout.hpp
    PlayButton.cpp
    PlayButton.hpp
    SeekBar.cpp
    SeekBar.hpp
    Spacer.cpp
    Spacer.hpp
    Types.hpp
    UiElement.cpp
    UiElement.hpp
)
target_link_libraries(graphics PUBLIC core sfml-graphics)
target_include_directories(graphics
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}    
)
