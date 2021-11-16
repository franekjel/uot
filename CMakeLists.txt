cmake_minimum_required(VERSION 3.15)

project(uot LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(FETCHCONTENT_QUIET OFF)
include(FetchContent)
FetchContent_Declare(
  json
  GIT_REPOSITORY "https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent"
  GIT_TAG        "v3.9.1"
)
FetchContent_GetProperties(json)
if(NOT json_POPULATED)
  FetchContent_Populate(json)
  add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()
FetchContent_MakeAvailable(json)

if(WIN32 AND NOT MSYS2_BUILD) # on windows we use vcpkg on other platforms fetchcontent
  find_package(GameNetworkingSockets)
else()
  FetchContent_Declare(
    net
    GIT_REPOSITORY "https://github.com/ValveSoftware/GameNetworkingSockets"
    GIT_TAG        "6c0cea36abc5b7656d91dd0ba96c9387e5a87197"
  )
  FetchContent_GetProperties(net)
  if(NOT net_POPULATED)
    FetchContent_Populate(net)
    add_subdirectory(${net_SOURCE_DIR} ${net_BINARY_DIR} EXCLUDE_FROM_ALL)
  endif()
  FetchContent_MakeAvailable(net)
  set_target_properties(GameNetworkingSockets PROPERTIES
    CXX_EXTENSIONS ON
  )
endif()


if(MSYS2_BUILD)
  target_link_libraries(GameNetworkingSockets PUBLIC winmm ws2_32)
endif()

add_subdirectory(uot)
add_subdirectory(uot-server)
add_subdirectory(libuot)
