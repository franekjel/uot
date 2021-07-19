cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 17)

find_package(Qt5 REQUIRED COMPONENTS 3DCore 3DRender 3DInput 3DLogic 3DAnimation)


set(SOURCES
    ./imgui/imgui.cpp
    ./imgui/imgui_draw.cpp
    ./imgui/imgui_demo.cpp
    ./imgui/imgui.h
    imguimanager.cpp
    imguiqt3dwindow.cpp
)


add_library(imgui-qt3d ${SOURCES} )

target_link_libraries(imgui-qt3d
    Qt5::3DCore
    Qt5::3DRender
    Qt5::3DInput
    Qt5::3DLogic
    Qt5::3DAnimation
)
