cmake_minimum_required(VERSION 3.7)

project(UOT)


set(SRCS
    include/assets.h
    include/game_renderer.h
    include/game_resources.h
    include/game_state.h
    include/sdl_utilities.h
    include/singleton.h
    include/size_settings.h
    include/uncopiable.h

    src/game_renderer.cpp
    src/game_state.cpp
    src/sdl_utilities.cpp
    src/main.cpp
)

add_executable(uot ${SRCS})


find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)


target_link_libraries(uot SDL2::Main SDL2::Image)
