set(APP_NAME "uot")
set(SRCS
    include/assets.h
    include/game_rendering.h
    include/resource_manager.h
    include/game_state.h
    include/sdl_utilities.h
    include/singleton.h
    include/size_settings.h
    include/uncopiable.h

    src/input_handlers.cpp
    src/game_rendering.cpp
    src/game_state.cpp
    src/sdl_utilities.cpp
    src/main.cpp
)


include_directories(include/)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

add_executable(${APP_NAME} ${SRCS})

target_link_libraries(${APP_NAME} SDL2::Main SDL2::Image)
target_link_libraries(${APP_NAME} libuot)

add_custom_command(
        TARGET ${APP_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/assets/
                ${CMAKE_CURRENT_BINARY_DIR}/assets/)

set(NET_TEST_SRCS
    include/net/client_txrx.h
    src/net/client_txrx.cpp src/net/net_client_mock.cpp
)
add_executable(NetTest ${NET_TEST_SRCS})
target_link_libraries(NetTest GameNetworkingSockets::GameNetworkingSockets)
