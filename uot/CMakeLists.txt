set(APP_NAME "uot")
set(SRCS
    include/constants/assets.h
    include/rendering/gui/buttons.h
    include/utilities/sdl_include.h
    include/client_context.h
    include/game_gui.h
    include/game_resources.h
    include/game_state.h
    include/utilities/input_utilities.h
    include/net/client_txrx.h
    include/rendering/rendering.h
    include/rendering/rendering_all_views.h
    include/rendering/rendering_common.h
    include/rendering/rendering_sector.h
    include/rendering/rendering_menu.h
    include/rendering/rendering_planet.h
    include/rendering/rendering_universe.h
    include/rendering/rendering_tech.h
    include/rendering/rendering_views.h
    include/rendering/ui_list.h
    include/utilities/sdl_utilities.h
    include/utilities/setup_utils.h
    include/singleton.h
    include/constants/size_settings.h
    include/rendering/texture.h
    include/uncopiable.h
    include/net/uot_net_client.h
    include/msg_queue.h

    src/game_state.cpp
    src/utilities/input_utils.cpp
    src/main.cpp
    src/net/client_txrx.cpp
    src/rendering/rendering_common.cpp
    src/rendering/rendering_sector.cpp
    src/rendering/rendering_menu.cpp
    src/rendering/rendering_planet.cpp
    src/rendering/rendering_universe.cpp
    src/rendering/rendering_tech.cpp
    src/rendering/ui_list.cpp
    src/rendering/buttons.cpp
    src/utilities/sdl_utils.cpp
    src/utilities/setup_utils.cpp
    src/net/uot_net_client.cpp
    src/msg_queue.cpp)


include_directories(include/)
include_directories(include/rendering)
include_directories(include/constants)
include_directories(include/utilities)
include_directories(include/net)
include_directories(include/gui)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)

add_executable(${APP_NAME} ${SRCS})

target_link_libraries(${APP_NAME} SDL2::Main SDL2::Image SDL2::TTF SDL2::Mixer)
target_link_libraries(${APP_NAME} libuot)
target_link_libraries(${APP_NAME} GameNetworkingSockets::GameNetworkingSockets)


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
