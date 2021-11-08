cmake_minimum_required(VERSION 3.15)

project(uot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

IF(MSYS2_BUILD)
  set(CMAKE_VERBOSE_MAKEFILE ON)
  set(CMAKE_CXX_STANDARD_REQUIRED OFF)
ENDIF()

#IF(MSYS2_BUILD)
#message("${CMAKE_MODULE_PATH}")
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
#message("${CMAKE_MODULE_PATH}")
#ELSE()
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
#ENDIF()

set(FETCHCONTENT_QUIET OFF)
include(FetchContent)
FetchContent_Declare(
  json
  GIT_REPOSITORY "https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent"
  GIT_TAG        "v3.9.1"
)
FetchContent_Declare(
        net
        GIT_REPOSITORY "https://github.com/ValveSoftware/GameNetworkingSockets"
        GIT_TAG        "6c0cea36abc5b7656d91dd0ba96c9387e5a87197"
)
FetchContent_GetProperties(json)
if(NOT json_POPULATED)
  FetchContent_Populate(json)
  add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()
FetchContent_MakeAvailable(json)

FetchContent_GetProperties(net)
if(NOT net_POPULATED)
  FetchContent_Populate(net)
  add_subdirectory(${net_SOURCE_DIR} ${net_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()
FetchContent_MakeAvailable(net)

set_target_properties(GameNetworkingSockets PROPERTIES
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS ON
        )
IF (WIN32)
  IF (MSYS2_BUILD)
    target_link_libraries(GameNetworkingSockets PUBLIC winmm ws2_32)
    ELSE()
    target_link_libraries(GameNetworkingSockets PUBLIC winmm.lib ws2_32.lib)
    ENDIF()
ENDIF()

add_subdirectory(uot)
add_subdirectory(uot-server)
add_subdirectory(libuot)
