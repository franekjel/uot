set(APP_NAME "uot-server")

#here we list sources (and headers). If there is so many of them maybe create new module - with new folder and new list - MODULE_SRCS
set(SRCS main.cpp)

add_executable(${APP_NAME}
    ${SRCS}
)

target_link_libraries(${APP_NAME}
    PRIVATE
    libuot
)
