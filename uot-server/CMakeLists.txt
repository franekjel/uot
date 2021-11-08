cmake_minimum_required(VERSION 3.7)
set(APP_NAME "uot-server")
project(UOT_SERVER)
cmake_policy(SET CMP0079 NEW)

#here we list sources (and headers). If there is so many of them maybe create new module - with new folder and new list - MODULE_SRCS
set(SRCS main.cpp players.h)

set(NET_TEST_SRCS
        net/server_txrx.cpp
        net/server_txrx.h
        net/net_server_mock.cpp
        net/net_server_mock.h)

add_executable(${APP_NAME}
    ${SRCS}
)

add_executable(ServerNetTest
        ${NET_TEST_SRCS}
        )

target_link_libraries(${APP_NAME}
    PRIVATE
    libuot
)
include_directories(${net_SOURCE_DIR}/include)
target_link_libraries(ServerNetTest GameNetworkingSockets::GameNetworkingSockets)