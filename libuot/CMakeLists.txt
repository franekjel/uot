set(LIB_NAME "libuot")

#here we list sources (and headers). If there is so many of them maybe create new module - with new folder and new list - MODULE_SRCS
set(SRCS
    ship.h
    ship.cpp
    galaxy.h
    sectorobject.h
    sector.h
    planet.h
    resource.h
    galaxygenerator.h
    galaxygenerator.cpp
    resource.cpp
    common.h
    player.h
    player.cpp
    technology.h
    structs.h
    spacebase.h
    planet.cpp
    msg/messagetypes.h
    msg/messagetypes.cpp
    msg/messagestructs.h
    msg/messagestructs.cpp
)

add_library(${LIB_NAME}
    ${SRCS}
)

#this allow to include libuot headers from other modules (without prefixes)
target_include_directories(${LIB_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

#here we must specify all libraries we need
target_link_libraries(${LIB_NAME}
    PUBLIC
    nlohmann_json::nlohmann_json
)

set(MSG_TEST_SRCS
    msg/messagetypes.h
    msg/messagetypes.cpp
    msg/messagestructs.h
    msg/messagestructs.cpp
    msg/messagetypesTests.cpp
)

add_executable(MsgTest
        ${MSG_TEST_SRCS})

target_include_directories(MsgTest PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_link_libraries(MsgTest 
    PRIVATE libuot
    PUBLIC nlohmann_json::nlohmann_json)
