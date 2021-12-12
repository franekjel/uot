set(APP_NAME "uot")
set(SRCS
    include/assets.h
    include/rendering.h
    include/game_resources.h
    include/game_state.h
    include/sdl_utilities.h
    include/singleton.h
    include/size_settings.h
    include/uncopiable.h
    include/rendering_menu.h
    include/rendering_universe.h
    include/rendering_galaxy.h
    include/rendering_planet.h
    include/client_context.h
    include/game_gui.h
    include/rendering_views.h
    include/rendering_common.h
    include/setup_utils.h
    include/rendering_all_views.h
    include/input_utilities.h
    include/ui_button.h
    include/texture.h
    include/ui_menu_lists.h

    src/input_utilities.cpp
    src/game_state.cpp
    src/sdl_utilities.cpp
    src/setup_utils.cpp
    src/rendering_universe.cpp
    src/rendering_galaxy.cpp
    src/rendering_menu.cpp
    src/rendering_common.cpp
    src/main.cpp
)


include_directories(include/)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

add_executable(${APP_NAME} ${SRCS})

target_link_libraries(${APP_NAME} SDL2::Main SDL2::Image SDL2::TTF)
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
