cmake_minimum_required(VERSION 3.10)
project(tetris)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SDL2 and SDL2_ttf
find_package(SDL2 REQUIRED)

# Use pkg-config for SDL2_ttf
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)

add_executable(tetris
    src/main.cpp
    src/Game.cpp
    src/Board.cpp
    src/Piece.cpp
    src/Renderer.cpp
    src/InputHandler.cpp
)

target_include_directories(tetris PRIVATE 
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_TTF_INCLUDE_DIRS}
)

target_link_libraries(tetris 
    ${SDL2_LIBRARIES}
    ${SDL2_TTF_LIBRARIES}
)

# Windows-specific settings
if(WIN32)
    target_link_libraries(tetris SDL2main)
endif()
