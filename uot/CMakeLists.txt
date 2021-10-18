set(APP_NAME "uot")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})


#here we list sources (and headers). If there is so many of them maybe create new module - with new folder and new list - MODULE_SRCS
set(SRCS
    main.cpp
)

add_executable(${APP_NAME}
    ${SRCS}
)

#here we must specify all libraries we need
target_link_libraries(${APP_NAME}
    libuot
    ${SDL2_LIBRARIES}
)
