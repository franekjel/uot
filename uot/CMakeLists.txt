set(APP_NAME "uot")

find_package(Qt5 REQUIRED COMPONENTS Core Gui 3DCore 3DRender 3DInput 3DLogic 3DExtras)

#here we list sources (and headers). If there is so many of them maybe create new module - with new folder and new list - MODULE_SRCS
set(SRCS main.cpp)

add_executable(${APP_NAME}
    ${SRCS}
)

#this allow to shorts include, eg. you don't need to write #include "imgui-qt3d/imgui/imgui.h" but just "imgui.h"
target_include_directories(${APP_NAME}
    PRIVATE
    "imgui-qt3d/"
    "imgui-qt3d/imgui"
)
add_subdirectory(imgui-qt3d)

#here we must specify all libraries we need
target_link_libraries(${APP_NAME}
    PRIVATE
    Qt5::3DCore
    Qt5::3DRender
    Qt5::3DInput
    Qt5::3DLogic
    Qt5::3DExtras
    imgui-qt3d
    libuot
)
