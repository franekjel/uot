cmake_minimum_required(VERSION 3.7)

project(UOT)
cmake_policy(SET CMP0079 NEW)

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

if(MSYS2_BUILD)
#    add_definitions(-DSDL_MAIN_HANDLED)
    include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} ${net_SOURCE_DIR}/include)
ENDIF()

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

#IF(MSYS2_BUILD)
#    target_link_libraries(uot SDL2::SDL2 SDL2::SDL2main)
#ELSE()
    target_link_libraries(uot SDL2::Main SDL2::Image)
#ENDIF()

set(NET_TEST_SRCS
        include/net/client_txrx.h
        src/net/client_txrx.cpp src/net/net_client_mock.cpp)

add_executable(NetTest ${NET_TEST_SRCS})

target_link_libraries(NetTest GameNetworkingSockets::GameNetworkingSockets)
