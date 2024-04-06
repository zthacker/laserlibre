cmake_minimum_required(VERSION 3.27)
project(laserlibre)

set(CMAKE_CXX_STANDARD 17)

set(SDL2_INCLUDE_DIR C:/libs/SDL/include)
set(SDL2_LIB_DIR C:/libs/SDL/lib/x64)

set(SDL2_IMAGE_INCLUDE_DIR C:/libs/SDL_image/include)
set(SDL2_IMAGE_LIB_DIR C:/libs/SDL_image/lib/x64)

include_directories(${SDL2_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIR})
link_directories(${SDL2_LIB_DIR} ${SDL2_IMAGE_LIB_DIR})

add_executable(laserlibre main.cpp
        common.h
        defs.h
        Game.h
        Game.cpp
        Entity.h
        Entity.cpp
)

target_link_libraries(${PROJECT_NAME} SDL2main SDL2 SDL2_image)
