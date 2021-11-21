set(APP_NAME "uot-server")

include_directories(${net_SOURCE_DIR}/include)
set(SRCS
    main.cpp
    headers/players.h
    headers/server.h
    src/server.cpp
    src/players.cpp
    net/server_txrx.cpp
    net/server_txrx.h
    net/net_server_uot.cpp
    net/net_server_uot.h
)

add_executable(${APP_NAME}
    ${SRCS})

target_link_libraries(${APP_NAME}
    PRIVATE libuot
    GameNetworkingSockets::GameNetworkingSockets
)

set(NET_TEST_SRCS
        net/server_txrx.cpp
        net/server_txrx.h
        net/net_server_mock.cpp
        net/net_server_mock.h
)

add_executable(ServerNetTest
        ${NET_TEST_SRCS})

include_directories(${net_SOURCE_DIR}/include)
target_link_libraries(ServerNetTest GameNetworkingSockets::GameNetworkingSockets)
