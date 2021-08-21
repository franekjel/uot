set(LIB_NAME "libuot")

find_package(Qt5 REQUIRED COMPONENTS Core Gui 3DCore 3DRender 3DInput 3DLogic 3DExtras)

#here we list sources (and headers). If there is so many of them maybe create new module - with new folder and new list - MODULE_SRCS
set(SRCS
    libmain.h
    libmain.cpp
    ship.h
    ship.cpp
    galaxy.h
    sectorobject.h
    sector.h
    planet.h
    resource.h
    galaxygenerator.h
    galaxygenerator.cpp
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
    PRIVATE
    Qt5::3DCore
)
