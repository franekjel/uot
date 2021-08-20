set(LIB_NAME "libuot")

#here we list sources (and headers). If there is so many of them maybe create new module - with new folder and new list - MODULE_SRCS
set(SRCS
    libmain.h
    libmain.cpp
    ship.h
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
