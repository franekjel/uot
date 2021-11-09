set(APP_NAME "uot")
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

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

add_executable(${APP_NAME} ${SRCS})
target_link_libraries(${APP_NAME} SDL2::Main SDL2::Image)

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR}/${APP_NAME})

set(NET_TEST_SRCS
    include/net/client_txrx.h
    src/net/client_txrx.cpp src/net/net_client_mock.cpp
)
add_executable(NetTest ${NET_TEST_SRCS})
target_link_libraries(NetTest GameNetworkingSockets::GameNetworkingSockets)
